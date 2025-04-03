{-# LANGUAGE FlexibleContexts #-}
{-# LANGUAGE ImportQualifiedPost #-}
{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE RecordWildCards #-}
{-# LANGUAGE TypeApplications #-}

module Main where

import Control.Applicative
import Control.Concurrent (forkIO, threadDelay)
import Control.Concurrent.MVar (MVar, modifyMVar_, newMVar, readMVar)
import Control.Monad (forM_, forever, when)
import Control.Monad.Reader (ReaderT, ask, asks, lift, liftIO, runReaderT)
import Data.Functor ((<&>))
import Data.HashMap.Strict (HashMap)
import Data.HashMap.Strict qualified as HashMap
import Data.List (partition)
import Data.Text (Text)
import Data.Text qualified as Text
import Data.Time.Calendar
import Data.Time.Clock
import Data.Time.Format
import Data.Time.LocalTime
import Servant.Client (runClientM)
import Telegram.Bot.API
import Telegram.Bot.API (ChatId, SomeChatId (SomeChatId), updateChatId)
import Telegram.Bot.Simple
import Telegram.Bot.Simple.RunTG
import Telegram.Bot.Simple.UpdateParser
import Text.Read (readMaybe)

type Item = Text

data Reminder = Reminder
  { reminderText :: Text,
    reminderTime :: UTCTime,
    reminderChatId :: ChatId
  }

data Model = Model
  { todoLists :: HashMap Text [Item],
    currentList :: Text,
    pendingAction :: Maybe Action,
    reminders :: [Reminder],
    botToken :: Token
  }

defaultListName :: Text
defaultListName = "Default"

initialModel :: Token -> Model
initialModel token =
  Model
    { todoLists = HashMap.fromList [(defaultListName, [])],
      currentList = defaultListName,
      pendingAction = Nothing,
      reminders = [],
      botToken = token
    }

data Action
  = Start ChatId
  | AddItem Item
  | RemoveItem Item
  | SwitchToList Text
  | ShowAll
  | Show Text
  | AddReminder Text UTCTime ChatId
  | ShowReminders
  | DeleteReminder Int

todoBot3 :: Token -> IO (BotApp Model Action)
todoBot3 token = do
  stateVar <- newMVar (initialModel token)

  -- Запускаем фоновый поток для проверки напоминаний
  forkIO $ reminderCheckerThread stateVar token

  return $
    BotApp
      { botInitialModel = initialModel token,
        botAction = flip updateToAction,
        botHandler = handleAction stateVar,
        botJobs = []
      }
  where
    reminderCheckerThread :: MVar Model -> Token -> IO ()
    reminderCheckerThread stateVar token = forever $ do
      currentTime <- getCurrentTime
      model <- readMVar stateVar

      -- Вывод времени и текущих напоминаний для отладки
      putStrLn $ "Current time: " <> formatTime defaultTimeLocale "%d.%m %H:%M:%S" currentTime
      putStrLn "Current reminders:"
      forM_ (zip [0 ..] (reminders model)) $ \(i, Reminder {..}) -> do
        putStrLn $ show i <> ": " <> Text.unpack reminderText <> " at " <> formatTime defaultTimeLocale "%d.%m %H:%M:%S" reminderTime

      let (due, upcoming) = span ((<= currentTime) . reminderTime) (reminders model)

      forM_ due $ \rem -> do
        sendReminder token rem
        threadDelay 500000

      modifyMVar_ stateVar $ \m ->
        return m {reminders = upcoming}

      now <- getCurrentTime
      let timePassed = diffUTCTime now currentTime
          delay = max 0 (60 - realToFrac timePassed)
      threadDelay (round $ delay * 1000000)

    handleAction :: MVar Model -> Action -> Model -> Eff Action Model
    handleAction stateVar action model = case action of
      AddReminder text time chatId ->
        model <# do
          let newReminder = Reminder text time chatId
          liftIO $ modifyMVar_ stateVar $ \m ->
            return m {reminders = newReminder : reminders m}
          replyText "Добавила!"
      DeleteReminder idx ->
        model <# do
          liftIO $ modifyMVar_ stateVar $ \m ->
            return $ removeReminderByIdx idx m
          replyText "Удалила!"
      ShowReminders ->
        model <# do
          m <- liftIO $ readMVar stateVar
          let reminderTexts = map (\(i, reminder) -> Text.pack (show i) <> ": " <> reminderText reminder <> " at " <> (Text.pack $ formatTime defaultTimeLocale "%d.%m %H:%M" (reminderTime reminder))) (zip [0 ..] (reminders m))
          if null reminderTexts
            then replyText "Напоминаний нет 🤷"
            else replyText (Text.unlines reminderTexts)
      _ -> defaultHandler action model

    defaultHandler :: Action -> Model -> Eff Action Model
    defaultHandler action model = case action of
      Start chatId ->
        model <# do
          reply (toReplyMessage startMessage)
      AddItem item ->
        addItem item model <# do
          replyText "Оки!"
      RemoveItem item ->
        removeItem item model <# do
          replyText "Удалила!"
      SwitchToList name ->
        model {currentList = name} <# do
          replyText ("Переключаемся на список «" <> name <> "»!")
      ShowAll ->
        model <# do
          let lists = HashMap.keys (todoLists model)
              listsText =
                if null lists
                  then "Списков дел нет 🤷"
                  else "Твои списки дел:\n" <> Text.unlines (map (\name -> "- " <> name) lists)
          reply (toReplyMessage listsText) {replyMessageReplyMarkup = Nothing}
      Show "" ->
        model <# do
          return (Show defaultListName)
      Show name ->
        model <# do
          let items = concat (HashMap.lookup name (todoLists model))
          if null items
            then reply (toReplyMessage ("The list «" <> name <> "» is empty"))
            else replyText (Text.unlines items)
      _ ->
        model {pendingAction = Nothing} <# do
          replyText "Команда не распознана."

    updateToAction :: Model -> Update -> Maybe Action
    updateToAction _ upd = runReaderT parser upd
      where
        parser =
          AddItem <$> command "add"
            <|> (Start <$> (command "start" *> lift (updateChatId upd)))
            <|> RemoveItem <$> command "remove"
            <|> (DeleteReminder <$> (command "rmrem" >>= parseInt))
            <|> fmap (\(text, time, cid) -> AddReminder text time cid) (command "mkrem" >>= parseReminderWithChatId)
            <|> SwitchToList <$> command "switch_to_list"
            <|> Show <$> command "show"
            <|> ShowAll <$ command "show_all"
            <|> ShowReminders <$ command "show_reminders"

    parseReminderWithChatId :: Text -> ReaderT Update Maybe (Text, UTCTime, ChatId)
    parseReminderWithChatId msg = do
      upd <- ask
      case (Text.words msg, updateChatId upd) of
        ((dateStr : timeStr : rest), Just cid) -> do
          let currentYear = 2025
              timeStrWithYear = dateStr <> " " <> timeStr <> " " <> Text.pack (show currentYear)
          case parseTimeM True defaultTimeLocale "%d.%m %H:%M %Y" (Text.unpack timeStrWithYear) of
            Just time -> return (Text.unwords rest, time, cid)
            Nothing -> empty
        _ -> empty

    parseInt :: Text -> ReaderT Update Maybe Int
    parseInt txt = case readMaybe (Text.unpack txt) of
      Just n -> return n
      Nothing -> empty

    startMessage =
      Text.unlines
        [ Text.pack "Приветик! Я ящерка. Я буду приносить тебе напоминания и списки дел.",
          Text.pack "",
          Text.pack "Вот как ты можешь работать с НАПОМИНАНИЯМИ:",
          Text.pack "1. Напиши /mkrem, чтобы поставить напоминание. Помни! Напомиинания нужно поставить в виде DD.MM HH:MM Текст.",
          Text.pack "Я пока умею работать только с UTC временем :)",
          Text.pack "2. Напиши /show_reminders, чтобы посмотреть на список твоих напоминаний",
          Text.pack "3. Если хочешь удалить напоминание, напиши /rmrem и номер соответствующего напоминания в списке",
          Text.pack "",
          Text.pack "Вот так ты можешь работать с СПИСКАМИ ДЕЛ:",
          Text.pack "1. Напиши /add, чтобы добавить дело в твой список",
          Text.pack "2. Напиши /remove, чтобы удалить дело. Тут придется полностью написать его, извини :( Ящерица испугалась и забыла, как считать",
          Text.pack "3. Напиши switch_to_list <list>, чтобы переключиться между списками дел",
          Text.pack "4. Напиши /show_all, чтобы увидеть названия всех своих списков",
          Text.pack "5. Напиши /show <list>, чтобы посмотреть содержимое конкретного списка.",
          Text.pack ""
        ]


    addItem :: Item -> Model -> Model
    addItem item model =
      model
        { todoLists = HashMap.insertWith (++) (currentList model) [item] (todoLists model)
        }

    removeItem :: Item -> Model -> Model
    removeItem item model =
      model
        { todoLists = HashMap.adjust (filter (/= item)) (currentList model) (todoLists model)
        }

    removeReminderByIdx :: Int -> Model -> Model
    removeReminderByIdx idx model = model {reminders = take idx (reminders model) ++ drop (idx + 1) (reminders model)}

{-
sendReminder :: Token -> Reminder -> IO ()
sendReminder botToken reminder = do
  let request =
        SendMessageRequest
          { sendMessageChatId = SomeChatId (reminderChatId reminder),
            sendMessageText = "⏰ " <> reminderText reminder
          }
  env <- defaultTelegramClientEnv botToken
  _ <- runClientM (sendMessage request) env
  pure ()
-}
sendReminder :: Token -> Reminder -> IO ()
sendReminder botToken reminder = do
  let request =
        SendMessageRequest
          { sendMessageBusinessConnectionId = Nothing,
            sendMessageChatId = SomeChatId (reminderChatId reminder),
            sendMessageMessageThreadId = Nothing,
            sendMessageText = "⏰ " <> reminderText reminder,
            sendMessageParseMode = Nothing,
            sendMessageEntities = Nothing,
            sendMessageLinkPreviewOptions = Nothing,
            sendMessageDisableNotification = Nothing,
            sendMessageProtectContent = Nothing,
            sendMessageMessageEffectId = Nothing,
            sendMessageReplyToMessageId = Nothing,
            sendMessageReplyParameters = Nothing,
            sendMessageReplyMarkup = Nothing
          }
  env <- defaultTelegramClientEnv botToken
  _ <- runClientM (sendMessage request) env
  pure ()

main :: IO ()
main = do
  putStrLn "Enter bot token:"
  token <- Token . Text.pack <$> getLine
  app <- todoBot3 token
  env <- defaultTelegramClientEnv token
  startBot_ (conversationBot updateChatId app) env
{-# LANGUAGE ImportQualifiedPost #-}
{-# LANGUAGE OverloadedStrings #-}

module Main where

import Control.Monad (forever)
import Telegram.Bot.API.Methods.SendMessage 
import Control.Concurrent (threadDelay)
import Control.Concurrent (forkIO)
import Control.Applicative
import Data.Text (Text)
import qualified Data.Text as Text
import Data.HashMap.Strict (HashMap)
import qualified Data.HashMap.Strict as HashMap
import Data.Time.Clock
import Data.Time.Format
import Telegram.Bot.API
import Telegram.Bot.Simple
import Telegram.Bot.Simple.UpdateParser
import Control.Monad.Trans.Reader (ReaderT)
import Text.Read (readMaybe)

type Item = Text

data Reminder = Reminder
  { reminderText :: Text
  , reminderTime :: UTCTime
  }

data Model = Model
  { todoLists   :: HashMap Text [Item]
  , currentList :: Text
  , pendingAction :: Maybe Action
  , reminders   :: [Reminder]
  }

defaultListName :: Text
defaultListName = "Default"

initialModel = Model
  { todoLists = HashMap.fromList [(defaultListName, [])]
  , currentList = defaultListName
  , pendingAction = Nothing
  , reminders = []
  }

data Action
  = Start
  | AddItem Item
  | RemoveItem Item
  | SwitchToList Text
  | ShowAll
  | Show Text
  | AddReminder Text UTCTime
  | ShowReminders 
  | DeleteReminder Int
  deriving (Show, Read)

-- Функция для парсинга числа (индекса)
parseInt :: Text -> ReaderT Update Maybe Int
parseInt txt = case readMaybe (Text.unpack txt) of
  Just n  -> return n  -- Просто возвращаем индекс
  Nothing -> empty

todoBot3 :: BotApp Model Action
todoBot3 = BotApp
  { botInitialModel = initialModel
  , botAction = flip updateToAction
  , botHandler = handleAction
  , botJobs = []
  }
  where
    updateToAction :: Model -> Update -> Maybe Action
    updateToAction _ = parseUpdate $
            AddItem      <$> command "add"
        <|> Start        <$  command "start"
        <|> RemoveItem   <$> command "remove"
        <|> (DeleteReminder <$> (command "rmrem" >>= parseInt)) -- Удаление по номеру будет реализовано в другом месте
        <|> fmap (uncurry AddReminder) (command "mkrem" >>= parseReminder)
        <|> SwitchToList <$> command "switch_to_list"
        <|> Show         <$> command "show"
        <|> ShowAll      <$  command "show_all"
        <|> ShowReminders <$  command "show_reminders" 
    -- Функция для парсинга сообщения с напоминанием в формате "DD.MM HH:MM Text"
    parseReminder :: Text -> ReaderT Update Maybe (Text, UTCTime)
    parseReminder msg = case Text.splitOn " " msg of
      [dateStr, timeStr, reminderText] -> case parseTimeM True defaultTimeLocale "%d.%m %H:%M" (Text.unpack (dateStr <> " " <> timeStr)) of
        Just time -> return (reminderText, time)
        Nothing   -> empty
      _ -> empty

    handleAction :: Action -> Model -> Eff Action Model
    handleAction action model = case action of
        AddItem item -> addItem item model <# do
            replyText "Ok, got it!" 
        RemoveItem item -> removeItem item model <# do
            replyText "Item removed!" 
        Start -> model <# do
            reply (toReplyMessage startMessage)  -- Проверяем, что кнопки здесь актуальны

        SwitchToList name -> model { currentList = name } <# do
            replyText ("Switched to list «" <> name <> "»!") 
            
        ShowAll -> model <# do
            reply (toReplyMessage "Available todo lists")  -- Отображаем клавиатуру только при необходимости
                { replyMessageReplyMarkup = Just (SomeInlineKeyboardMarkup listsKeyboard) }
                
        Show "" -> model <# do
            return (Show defaultListName)
                
        Show name -> model <# do
            let items = concat (HashMap.lookup name (todoLists model))
            if null items
            then reply (toReplyMessage ("The list «" <> name <> "» is empty"))
            else replyText (Text.unlines items)

        AddReminder text time -> model { reminders = reminders model ++ [Reminder text time] } <# do
            replyText "Reminder added!"    

        DeleteReminder idx -> removeReminderByIdx idx model <# do
            replyText "Reminder removed!" 

        ShowReminders -> model <# do
            let reminderTexts = map (\(i, reminder) -> Text.pack (show i) <> ": " <> reminderText reminder <> " at " <> (Text.pack $ formatTime defaultTimeLocale "%d.%m %H:%M" (reminderTime reminder))) (zip [0..] (reminders model))
            if null reminderTexts
            then replyText "You have no reminders."
            else replyText (Text.unlines reminderTexts)

        _ -> model { pendingAction = Nothing } <# do 
            replyText "Команда не распознана."

        where
         listsKeyboard = InlineKeyboardMarkup
            (map (\name -> [actionButton name (Show name)]) (HashMap.keys (todoLists model)))



    startMessage = Text.unlines
      [ "Hello! I am a Lizard! I will bring you your reminders and TODO lists"
      , ""
      , "Here, look at your buttons!"
      , "You can also use /add command to do that explicitly."
      , "To remove an item use /remove command."
      , ""
      , "You can manage multiple todo lists:"
      , "Switch to a new named list with /switch_to_list <list>."
      , "Show all available lists with /show_all."
      , "Show items for a specific list with /show <list>."
      , ""
      ]

    -- Добавление элемента в список
    addItem :: Item -> Model -> Model
    addItem item model = model
      { todoLists = HashMap.insertWith (++) (currentList model) [item] (todoLists model) }

    -- Удаление элемента из списка
    removeItem :: Item -> Model -> Model
    removeItem item model = model
      { todoLists = HashMap.adjust (filter (/= item)) (currentList model) (todoLists model)}

    -- Удаление напоминания по индексу
    removeReminderByIdx :: Int -> Model -> Model
    removeReminderByIdx idx model = model { reminders = take idx (reminders model) ++ drop (idx + 1) (reminders model)}

-- Запуск проверки напоминаний
checkReminders :: Model -> Token -> IO ()
checkReminders model token = forever $ do
  currentTime <- getCurrentTime
  let dueReminders = filter (\rem -> reminderTime rem <= currentTime) (reminders model)
  mapM_ (\rem -> sendReminder token rem) dueReminders
  threadDelay 60000000  -- 1 минута (60 секунд * 1000000 микросекунд)

sendReminder :: Token -> Reminder -> IO ()
sendReminder token reminder = do
  let msg = Text.concat ["Reminder: ", reminderText reminder]
  env <- defaultTelegramClientEnv token
  _ <- sendMessage env (SendMessage (ChatId "<your-chat-id>") msg Nothing Nothing Nothing)
  putStrLn ("Reminder sent: " <> Text.unpack msg)

run :: Token -> IO ()
run token = do
  env <- defaultTelegramClientEnv token
  forkIO $ checkReminders initialModel token
  startBot_ (conversationBot updateChatId todoBot3) env

main :: IO ()
main = getEnvToken "TELEGRAM_BOT_TOKEN" >>= run

-- in bush do
-- export TELEGRAM_BOT_TOKEN="7805803751:AAEEkfOf7v2Ocu_wqHX2CVYJ8XPWF_QkoOE"
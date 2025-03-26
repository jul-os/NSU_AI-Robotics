{-# LANGUAGE ImportQualifiedPost #-}

module Main (main) where

import Data.Text
import Telegram.Bot.API qualified as Telegram
import Telegram.Bot.Simple
import Telegram.Bot.Simple.Debug
import Telegram.Bot.Simple.UpdateParser

data Model = Model
  { todoItems :: [TodoItem] -- Теперь `todoItems` — это поле в `Model`
  }
  deriving (Show)

type TodoItem = Text

addItem :: TodoItem -> Model -> Model
addItem item model =
  model {todoItems = item : todoItems model}

data Action
  = DoNothing
  | -- | Echo Text
    AddItem Text -- will save it
  | ShowItems
  deriving (Show)

bot :: BotApp Model Action
bot =
  BotApp
    { botInitialModel = Model [],
      botAction = flip handleUpdate,
      botHandler = handleAction,
      botJobs = []
    }

handleUpdate :: Model -> Telegram.Update -> Maybe Action
handleUpdate _model =
  parseUpdate -- _model - to ignore
    (AddItem <$> text)
    <|> ShowItems <$> command "show"

--    (Echo <$> text) -- yo handle an update we extract a text from it and wrap it in an echo
-- Echo :: Text -> Action

handleAction :: Action -> Model -> Eff Action Model
-- produces a new action
handleAction action model =
  case action of
    DoNothing -> pure model
    -- returns model as is
    AddItem title ->
      addItem title model <# do
        replyText (pack "Got it!")
        pure DoNothing

{-
    Echo message ->
      model <# do
        -- here actions are performed in some separate thing
        replyText message
        -- takes text value and replies, also taken chat and message context
        -- and any impure stuff should result with an action so we do this
        pure DoNothing
    -- выводит hello world!!! ура
-}
-- take the model from the left and do model on the right
-- so we keep model away from all impure stuff

run :: Telegram.Token -> IO ()
run token = do
  env <- Telegram.defaultTelegramClientEnv token -- takes token and creates http client manager and where to send requests
  startBot_ (traceBotDefault bot) env -- так показывает логи сообщений, прикол
  -- startBot_ bot env

-- learn to reply fixed text to the user

main :: IO ()
main = getEnvToken "TELEGRAM_BOT_TOKEN" >>= run

-- can try to  run "tokennumsarljlajrlj" and it should run



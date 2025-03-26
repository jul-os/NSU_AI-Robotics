import Telegram.Bot.Simple
import qualified Telegram.Bot.API as Telegram

data Model = Model

data Action 
    = DoNothing

bot :: BotApp Model Action
bot = BotApp
    {botInitialModel = Model
    , botAction = handleUpdate
    , botHandlet = handleAction
    , botJobs = []
    }

handleUpdate :: Telegram.Update -> Model -> Maybe Action
handleUpdate _ _ = Nothing --not hoing to process this update

handleAction :: Action -> Model -> Eff Action Model
-- produces a new action
handleAction _ model = pure model --returns model as is

run :: Telegram.Token -> IO()
run token = do
    env <- Telegram.defaultTelegramClientEnv token 
    startBot_ (traceBotDefault bot) env

main :: IO ()
main = putStrLn "This bot is not implemented yet!"
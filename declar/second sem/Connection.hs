{-

module Connection where

{-# LANGUAGE OverloadedStrings #-}

import Network.HTTP.Simple
import System.Environment (lookupEnv)
import Configuration.Dotenv (loadFile, defaultConfig)
import Data.Aeson (FromJSON, ToJSON, decode, encode, (.:), (.=), object)
import Data.Aeson.Types (parseJSON, withObject, Parser)
import Data.Aeson.Key (fromString)
import GHC.Generics (Generic)
import qualified Data.ByteString.Char8 as BS
import qualified Data.ByteString.Lazy.Char8 as BSL
import qualified Data.ByteString.Lazy as BL  -- BL.ByteString (JSON-ответ)


-- Структура ответа от Telegram API
data Update = Update
    { updateMessage :: Maybe Message
    } deriving (Show)

instance FromJSON Update
instance ToJSON Update

data Message = Message
    { messageChatId :: Int
    , messageText :: String
    } deriving (Show)

instance FromJSON Message where
    parseJSON = withObject "Message" $ \v -> do
        chat <- v .: fromString "chat"
        text <- v .: fromString "text"
        chatId <- chat .: fromString "id"
        return $ Message chatId text

instance ToJSON Message
        
-- Функции для получения данных
getChatId :: Update -> Int
getChatId update = messageChatId (updateMessage update)

getMessageText :: Update -> String
getMessageText update = messageText (updateMessage update)

-- Функция для получения токена бота из переменных окружения
getBotToken :: IO (Maybe String)
getBotToken = do
    loadFile defaultConfig  -- Загружаем переменные из .env
    lookupEnv "TELEGRAM_BOT_TOKEN"  -- Ищем переменную

-- Функция для получения обновлений от бота
getUpdates :: IO ()
getUpdates = do
    maybeToken <- getBotToken
    case maybeToken of
        Nothing -> putStrLn "Ошибка: токен не найден!"
        Just botToken -> do
            let url = "https://api.telegram.org/bot" ++ botToken ++ "/getUpdates"
            request <- parseRequest url
            response <- httpLBS request
            let responseBody = getResponseBody response
            print responseBody  -- Вывод JSON-ответа в консоль
            processUpdates botToken responseBody  -- Обрабатываем полученные обновления


-- Обработка входящих сообщений
processUpdates :: String -> BL.ByteString -> IO ()
processUpdates botToken responseBody = do
    case decode responseBody of
        Nothing -> putStrLn "Ошибка: не удалось разобрать JSON!"
        Just updates -> mapM_ (handleMessage botToken) updates

-- Обработка отдельного сообщения
handleMessage :: String -> Update -> IO ()
handleMessage botToken update = do
    let chatId = getChatId update
    let messageText = getMessageText update
    case messageText of
        Just "/start" -> sendMessage botToken chatId "Привет! Я ваш бот-напоминальник. Отправьте команду /help, чтобы узнать больше. Отправьте команду /set, чтобы поставить напоминание."
        _ -> return ()


-- Отправка сообщения пользователю
sendMessage :: String -> Int -> IO ()
sendMessage token chatId = do
    let url = "https://api.telegram.org/bot" ++ token ++ "/sendMessage"
        body = encode $ object [fromString "chat_id" .= chatId, fromString "text" .= ("Привет! Я ваш бот!" :: String)]
    request <- parseRequest url
    let request' = setRequestMethod (BS.pack "POST")
                 $ setRequestHeader (BS.pack "Content-Type") [BS.pack "application/json"]
                 $ setRequestBodyLBS body request
    response <- httpLBS request'
    print $ getResponseBody response
      -- Выводим ответ от Telegram

      -}
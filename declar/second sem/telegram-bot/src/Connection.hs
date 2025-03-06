module Connection where

{-# LANGUAGE OverloadedStrings #-}

import Network.HTTP.Simple
import System.Environment (lookupEnv)
import Configuration.Dotenv (loadFile, defaultConfig)

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
            print $ getResponseBody response  -- Вывод JSON-ответа
--видит сообщения. класс
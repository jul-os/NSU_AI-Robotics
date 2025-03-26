{-# LANGUAGE OverloadedStrings #-}

module Reminder where 

import Control.Concurrent (forkIO, threadDelay)
import Control.Monad (forever)
import Data.Aeson (object, (.=))
import Data.Time.Clock (getCurrentTime)
import Data.Time.Format (defaultTimeLocale, formatTime, parseTimeM)
import Database.SQLite.Simple
import Network.HTTP.Simple
import EnvParse
import qualified System.Environment as Env



-- Структура напоминания
data Reminder = Reminder
  { userId :: Integer
  , message :: String
  , remindTime :: String
  } deriving (Show)

initDB :: IO Connection
initDB = do
    conn <- open "reminders.db"
    execute_ conn "CREATE TABLE IF NOT EXISTS reminders (id INTEGER PRIMARY KEY, user_id INTEGER, message TEXT, remind_time TEXT)"
    return conn

-- Функция обработки callback-запросов от кнопок
handleCallbackQuery :: Connection -> String -> Integer -> String -> IO ()
handleCallbackQuery conn token uid callbackData
    | callbackData == "add_reminder" = do
        sendReminder token uid "Отправьте время и дату напоминания в формате HH:MM DD.MM и текст."
        -- Ожидаем ввода времени и текста от пользователя
        waitForReminderInput conn token uid
    | callbackData == "delete_reminder" = do
        putStrLn "Пользователь выбрал удаление напоминания"
        -- Тут логика выбора и удаления
    | callbackData == "edit_reminder" = do
        putStrLn "Пользователь выбрал редактирование напоминания"
        -- Тут логика выбора и редактирования
    | otherwise = return ()

-- Ожидание ввода напоминания от пользователя
waitForReminderInput :: Connection -> String -> Integer -> IO ()
waitForReminderInput conn token uid = do
    -- Для простоты, будем ждать ввода в формате "HH:MM DD.MM ТЕКСТ"
    -- Можно улучшить, добавив полноценный парсинг и валидацию
    putStrLn "Ожидаем ввода времени и текста напоминания"
    -- Пока просто ожидаем строку
    remindTime <- getLine 
    putStrLn "Теперь напишите текст напоминания"
    message <- getLine
    addReminder conn (Reminder uid message remindTime)
    sendReminder token uid $ "Напоминание создано на " ++ remindTime ++ ": " ++ message

-- Добавление напоминания в базу
addReminder :: Connection -> Reminder -> IO ()
addReminder conn (Reminder uid msg time) =
  execute conn "INSERT INTO reminders (user_id, message, remind_time) VALUES (?, ?, ?)" (uid, msg, time)

-- Отправка напоминания пользователю
sendReminder :: String -> Integer -> String -> IO ()
sendReminder token uid msg = do
  let url = "https://api.telegram.org/bot" ++ token ++ "/sendMessage"
      request = setRequestBodyJSON (object ["chat_id" .= uid, "text" .= msg]) $ parseRequest_ url
  _ <- httpNoBody request
  return ()

-- Получение напоминаний по времени
getReminders :: Connection -> String -> IO [(Integer, String)]
getReminders conn time =
  query conn "SELECT user_id, message FROM reminders WHERE remind_time = ?" (Only time)

-- Удаление напоминаний после отправки
deleteReminders :: Connection -> String -> IO ()
deleteReminders conn time =
  execute conn "DELETE FROM reminders WHERE remind_time = ?" (Only time)
{-
-- Фоновая проверка напоминаний
checkReminders :: Connection -> String -> IO ()
checkReminders conn token = forever $ do
  now <- getCurrentTime
  let currentTime = formatTime defaultTimeLocale "%H:%M %d.%m" now
  reminders <- getReminders conn currentTime

  mapM_ (\(uid, msg) -> sendReminder token uid msg) reminders
  deleteReminders conn currentTime  -- Удаляем отправленные напоминания
  
  threadDelay 60000000 -- Проверяем каждую минуту
-}
-- Обработка входящих сообщений
handleMessage :: Connection -> String -> String -> Integer -> IO ()
handleMessage conn token text uid =
  case words text of
    ("/remind":time:date:msg) -> do
      let remindTime = time ++ " " ++ date
      parsedTime <- parseTimeM True defaultTimeLocale "%H:%M %d.%m" remindTime
      addReminder conn (Reminder uid (unwords msg) remindTime)
      sendReminder token uid $ "Напоминание создано на " ++ remindTime
    _ -> sendReminder token uid "Используйте команду: /remind HH:MM DD.MM ТЕКСТ"

-- Фоновая проверка напоминаний
checkReminders :: Connection -> String -> IO ()
checkReminders conn token = forever $ do
  now <- getCurrentTime
  let currentTime = formatTime defaultTimeLocale "%H:%M %d.%m" now
  reminders <- getReminders conn currentTime

  -- Отправляем напоминания пользователям
  mapM_ (\(uid, msg) -> sendReminder token uid msg) reminders

  -- Удаляем отправленные напоминания
  mapM_ (\(_, _, time) -> deleteReminders conn time) reminders

  -- Пауза на 1 минуту
  threadDelay 60000000 -- Проверяем каждую минуту

main :: IO ()
main = do
  -- Указываем путь к файлу .env
  envFilePath <- return "/home/julia/Desktop/code/NSU/NSU_AI-Robotics/declar/second sem/telegram-bot/.env"
  
  -- Загружаем переменные из файла
  env <- loadEnvFile envFilePath

  -- Извлекаем переменную
  token <- env "TELEGRAM_BOT_TOKEN" -- или используйте envParse для валидации типов

  putStrLn $ "Бот запущен с токеном: " ++ token

  conn <- initDB
  -- Запуск фонового процесса проверки напоминаний
  _ <- forkIO (checkReminders conn token)

  -- Тут должен быть код получения сообщений от Telegram (опущен)
  putStrLn "Bot is running..."

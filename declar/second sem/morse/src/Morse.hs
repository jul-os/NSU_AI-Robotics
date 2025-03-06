module Morse 
  ( charToMorse
  , stringToMorse
  , morseToChar
  , morseToString
  , decodeMorse 
  ) where


import Data.Char
import Data.List
import qualified Data.Map  as M

type Morse = String

morseCodes :: M.Map Char Morse
morseCodes =
  M.fromList
    [ ('a', ".-"),
      ('b', "-..."),
      ('c', "-.-."),
      ('d', "-.."),
      ('e', "."),
      ('f', "..-."),
      ('g', "--."),
      ('h', "...."),
      ('i', ".."),
      ('j', ".---"),
      ('k', "-.-"),
      ('l', ".-.."),
      ('m', "--"),
      ('n', "-."),
      ('o', "---"),
      ('p', ".--."),
      ('q', "--.-"),
      ('r', ".-."),
      ('s', "..."),
      ('t', "-"),
      ('u', "..-"),
      ('v', "...-"),
      ('w', ".--"),
      ('x', "-..-"),
      ('y', "-.--"),
      ('z', "--.."),
      ('1', ".----"),
      ('2', "..---"),
      ('3', "...--"),
      ('4', "....-"),
      ('5', "....."),
      ('6', "-...."),
      ('7', "--..."),
      ('8', "---.."),
      ('9', "----."),
      ('0', "-----"),
      (' ', "   ")
    ]

-- добавила пробел, чтобы можно было переводить и фразы\текст

charToMorse :: Char -> Maybe Morse
charToMorse letter = M.lookup (if isAlpha letter then toLower letter else letter) morseCodes

stringToMorse :: String -> Maybe [Morse]
stringToMorse word = if word == "" then Nothing else mapM charToMorse word

{-Function: 	mapM
Type: 	Monad a => (b -> a c) -> [b] -> a [c]-}

morseToChar :: Morse -> Maybe Char
morseToChar letter = fmap fst $ find ((== letter) . snd) (M.toList morseCodes)
-- M.toList возвращает пары расшифровка - морзе
-- ищем чтобы морзе совпадал с нашим
-- вытаскиваем первый элемент нужной пары
{-
Functor — это тип, который позволяет применять функцию к содержимому обёрнутому в какой-то контейнер.
Пусть есть контейнер с элементом, например, список или Maybe. 
С помощью Functor можно применить функцию ко всем элементам внутри этого контейнера, не изменяя сам контейнер.
fmap fst извлекает первый символ из пары, но оборачивает результат в Maybe, чтобы обработать случаи, когда ничего не найдено
-}

morseToString :: [Morse] -> Maybe String
morseToString word = if null word then Nothing else mapM morseToChar word

-- вот эта вся ужасная надстройка чтобы словами можно было пробелами разделять. 
-- как ее сделать с типом как в задании пока вопрос, а я уже не соображаю

decodeMorse :: String -> String
decodeMorse str =
  let tokens = splitMorse str
      decoded = mapM decodeToken tokens
   in case decoded of
        Just result -> concat result
        Nothing -> ""

splitMorse :: String -> [String]
splitMorse "" = []
splitMorse s =
  case span (/= ' ') s of
    (morse, ' ' : ' ' : ' ' : rest) -> morse : " " : splitMorse rest -- три пробела пробел в тексте
    (morse, ' ' : rest) -> morse : splitMorse rest -- один пробел новая буква
    (morse, []) -> [morse]
--- span, applied to a predicate p and a list xs, returns a tuple where first element is the longest prefix 
--(possibly empty) of xs of elements that satisfy p and second element is the remainder of the list:

decodeToken :: String -> Maybe String
decodeToken " " = Just " " -- оставляем пробелы между словами
decodeToken morse = case morseToChar morse of
  Just c -> Just [c]
  Nothing -> Nothing

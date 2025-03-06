import Test.QuickCheck
import Morse
import Data.Char (toLower)

genAlphaString :: Gen String
genAlphaString = listOf1 $ elements (['a'..'z'] ++ ['A'..'Z'] ++ " ")
-- тут я при написании  забыла что в алфавите цифры тоже есть
{-
listOf1 :: Gen a -> Gen [a]

Generates a non-empty list of random length. The maximum length depends on the size parameter.
-}

-- перевод строки в Морзе и обратно
prop_roundTrip :: String -> Bool
prop_roundTrip text =
  case stringToMorse (map toLower text) of 
    Nothing -> True
    Just morse -> case morseToString morse of
                    Nothing -> False
                    Just result -> map toLower result == map toLower text 


prop_onlyAlpha :: String -> Bool
prop_onlyAlpha text = all (`elem` ['a'..'z'] ++ ['A'..'Z'] ++ " ") text 

-- тестирование одиночных символов (рандомных)
prop_roundTripSingle :: Char -> Bool
prop_roundTripSingle char =
  case stringToMorse [toLower char] of 
    Nothing -> True
    Just morse -> case morseToString morse of
                    Nothing -> False
                    Just result -> [toLower char] == result

-- проверка на одиночный пробел
prop_roundTripSingleSpace :: Bool
prop_roundTripSingleSpace =
  case stringToMorse " " of
    Nothing -> False
    Just morse -> case morseToString morse of
                    Nothing -> False
                    Just result -> result == " "

main :: IO ()
main = do
    -- проверка на допустимые символы нужна чтобы убедиться что случайные тесты которые генериируются соответсвуют требованиям
    -- наверное
    quickCheck (forAll genAlphaString prop_onlyAlpha)  
    -- проверка на перевод туда обратно
    quickCheck (forAll genAlphaString prop_roundTrip)  
    -- проверка что одиночные символы переводит
    quickCheck prop_roundTripSingle
    -- проверка что один пробел пройдет
    quickCheck prop_roundTripSingleSpace

module Main (main) where
import System.Environment (getArgs)
import Morse

convertTo :: IO ()
convertTo = do
    text <- getLine
    let morseText = stringToMorse text
    case morseText of
        Nothing -> do
            putStrLn "null line!"
            return ()
        Just result -> do
            putStrLn (unwords result)
            convertTo

-- вот это с надстройкой для пробелов
convertFrom :: IO ()
convertFrom = do
    morse <- getLine
    let text = decodeMorse morse
    if null text
        then do
            putStrLn "null line!"
            return ()
        else do
            putStrLn text
            convertFrom

-- вот это без

{-
convertFrom :: IO ()
convertFrom = do
    morse <- getLine
    let text = morseToString ( words morse)
    case text of
        Nothing -> do
            putStrLn "null line!"
            return ()
        Just result -> do
            putStrLn result
            convertFrom
-}
main :: IO ()
main = do
    what <- getArgs
    case what of
        ["to"] -> convertTo
        ["from"] -> convertFrom
        _ -> putStrLn "Usage: Main.exe [to|from]"

import Morse

while :: IO Bool -> IO () -> IO ()
while cond op = do
  a <- cond
  if a
    then do
      op
      while cond op
    else do
      putStrLn "quited"
      return ()


convertTo :: IO ()
convertTo = do
    text <- getLine
    let morseText = stringToMorse text
    case morseText of
        Nothing ->  do
            putStrLn "null line!"
            return ()
        Just result -> do
            putStrLn (unwords result)
            convertTo

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

main :: IO ()
main = do
    what <- getLine
    case what of
        "to" -> do
            convertTo
        "from" ->
            convertFrom
        _ -> putStrLn "Usage: Main.exe [to|from]"

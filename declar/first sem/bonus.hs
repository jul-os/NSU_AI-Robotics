import System.IO (readFile)
import Text.Regex.Posix ((=~))

findWordsInBraces :: String -> [String]
findWordsInBraces content = do
  let regex = "(\\{+)([a-zA-Z0-9_]+)(\\}+)"
  let matches = content =~ regex :: [[String]] --создает массив слов выделенных в скобки вообще
  [word | [_, open, word, close] <- matches, length open == length close] --проверяет что количество скобок совпадает
  --пусть есть слово {{{word}}}, тогда получится [["{{{word}}}", "{{{", "word", "}}}"]]
  -- _ - полное слово
  -- =~ эта штука вроде бы разбивает на группы, которые потом и будут open, word, close

main :: IO ()
main = do
  content <- readFile "text.txt"
  let wordsInBraces = findWordsInBraces content
  mapM_ putStrLn wordsInBraces

{-
ghci> main

    instance
    contained
    drooping
    stake
    singer
    snuffed
    wages
    undigested
    cable
    bonneted
    grasp
    sigh
    freer
    leading
    lettuce
    individually
    immediate
    measuring
    alarm
    core
    transformation
    dogged
    peck
    capable
    decked
    extent
    encouraged
    remained
    drawing
    grade
    stretching
    knee
    stately
    attach
    obscure
    recognised
    fool
    feels
    hastened
    reversed
    thank
    
ghci> 
-}
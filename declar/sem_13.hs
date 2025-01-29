produceTwo :: IO Int
produceTwo = do
  return 1
  return 2

{-Определите действие ввода-вывода readUntil f, которое
читает строки от пользователя и возвращает их в виде списка.
Чтение прекращается, когда f возвращает True для какой-либо
строки. (Значение, для которого f возвращает True, не
возвращается).-}

readUntil :: (String -> Bool) -> IO [String]
readUntil f = do
  line <- getLine
  if f line
    then
      return []
    else do
      lines <- readUntil f
      return (line : lines)

while :: IO Bool -> IO () -> IO ()
while cond op = do
  a <- cond
  if a
    then do
      op
      while cond op
    else
        do
        putStrLn "quited"
        return ()

main :: IO ()
main = while(do
            putStrLn "do you want to quit?"
            a <- getLine
            return (a /= "yes")
    )
    (putStrLn " ")
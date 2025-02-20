import Data.Binary (encode)
import Data.Char (isUpper, toUpper)
import Distribution.Simple.Utils (xargs)
import System.Posix (accessModes)

group' :: (Eq a) => [a] -> [[a]]
group' x = foldr f [] x
  where
    f a [] = [[a]]
    f a ((y : ys) : yss)
      | a == y = (a : y : ys) : yss
      | otherwise = [a] : ((y : ys) : yss)

group'' :: (Eq a) => [a] -> [[a]]
group'' [] = [[]]
group'' (a : []) = [[a]]
group'' (x : xs)
  | x == y = (x : y : ys) : yss
  | otherwise = [[x]] ++ ((y : ys) : yss)
  where
    ((y : ys) : yss) = group'' xs

group''' :: (Eq a) => [a] -> [[a]]
group''' [] = []
group''' (x : xs) = [(takeWhile (== x) (x : xs))] ++ (group''' (nextgroup x xs))
  where
    nextgroup _ [] = []
    nextgroup y (ys : yss)
      | y == ys = nextgroup ys yss
      | otherwise = (ys : yss)

{-
Напишите функцию, реализующую алгоритм сжатия данных, заменяющий
повторяющиеся символы (серии) на один символ и число его повторов.
Функция должна преобразовать строку в список пар:
(<кол-во повторов>, <символ>).
Например:
ghci> encode "aaaabccaadeeee"
[(4,'a'),(1,'b'),(2,'c'),(2,'a'),(1,'d'),(4,'e’)]
-}

my_encode :: (Eq a) => [a] -> [(Int, a)]
my_encode x = map (\xs -> (length xs, head xs)) (group''' x)

{-
Напишите функцию, которая будет декодировать строку, закодированную
предыдущей функцией. Например:
ghci> my_decode [(4,'a'),(1,'b'),(2,'c'),(2,'a'),(1,'d'),(4,'e')]
"aaaabccaadeeee"
-}

my_decode :: (Eq a) => [(Int, a)] -> [a]
my_decode x = concatMap f x
  where
    f (0, _) = []
    f (1, x) = [x]
    f (n, x) = [x] ++ f ((n - 1), x)

{-
Напишите функцию groupElems которая группирует подряд идущие
одинаковые элементы в списке и возвращает список таких групп.
GHCi> groupElems []
[]
GHCi> groupElems [1,2]
[[1],[2]]
GHCi> groupElems [1,2,2,2,4]
[[1],[2,2,2],[4]]
GHCi> groupElems [1,2,3,2,4]
[[1],[2],[3],[2],[4]]
-}

group_el :: (Eq a) => [a] -> [[a]]
group_el x = foldr f [] x
  where
    f a [] = [[a]]
    f a ((x : xs) : xss)
      | a == x = (a : x : xs) : xss
      | otherwise = [a] : ((x : xs) : xss)

capitalize :: String -> String
capitalize x = unwords (map (\xs -> toUpper (head xs) : tail xs) (words x))

isAllUpper :: String -> Bool
isAllUpper word = all isUpper word

dellAllUpper :: String -> String
dellAllUpper x = unwords (foldr f [] (words x))
  where
    f word acc
      | isAllUpper word = acc
      | otherwise = word : acc

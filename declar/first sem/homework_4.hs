-- 1
find_three :: [(Int, Int, Int)]
find_three = [(x, y, z) | z <- [1 ..], x <- [1 .. z], y <- [1 .. z], x * x + y * y == z * z, x < y]

-- где <- значит пренадлежит

-- 2
collatz :: Int -> [Int]
collatz 1 = [1] -- потому что закончиться все должно на 1, и я при
collatz n
  | n `mod` 2 == 0 = n : collatz (n `div` 2)
  | otherwise = n : collatz (n * 3 + 1)

-- 2
{-
permutations :: [a] -> [[a]]
permutations [] = [[]]
permutations [a] = [[a]]
permutations [a, b] = [[a, b], [b, a]]
--permutations [a, b, c] = [[a, b, c], [a, c, b], [b, a, c], [b, c, a], [c, a, b], [c, b, a]]
permutations (x : xs) = [[x, xs], [y, xs], permutations(x : y : xs), permutations(y : x : xs)]
--ругается на возникновение y
-}

permutations :: [a] -> [[a]]
permutations [] = [[]]
permutations (x : xs) = [y | perm <- permutations xs, y <- insertions x perm]
  where
    {- в perm <- permutations xs проходим по всем перестановкам xs рекурсивно
    y - элемент какой-то который мы вставляем всемы способами в перестановки
    y <- insertions x perm — второй генератор, который проходит по всем способам вставки элемента x в перестановку perm.

    -}

    insertions :: a -> [a] -> [[a]]
    insertions x [] = [[x]]
    insertions x (y : ys) = (x : y : ys) : [y : zs | zs <- insertions x ys]

-- вычисляет все возможные споспобы вставить

-- 2
subsequences :: [a] -> [[a]]
subsequences [] = [[]]
-- lists are catenated with the ++ operator (списков!)
subsequences xs = foldr addSubsequences [[]] xs
  where
    addSubsequences x acc = acc ++ map (x :) acc

-- map выполняет то что в () для всего списка acc
-- то есть тут объединяет все что в acc с x
-- acc содержит все текущие подпоследовательности (включая пустую)

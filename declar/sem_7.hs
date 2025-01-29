{-
double all 3 разными способами
-}

doubleAll :: Num a => [a] -> [a]
doubleAll [] = []
doubleAll (x : xs) = (2 * x) : doubleAll xs

doubleAll' :: Num a => [a] -> [a]
doubleAll' xs = [2 * x | x <- xs]

doubleAll'' = map (* 2) --из-за каррирования можем оставить так потому что теперь map просто ждем список

length' :: [a] -> Int
length' x = sum (map (const 1) x)

reverse' :: [a] -> [a]
reverse' = foldr (\x acc -> acc ++ [x]) []

last' :: [a] -> a
last' = foldl1 (\_ x -> x)

{-
•Реализуйте функцию prefixes, используя
свертку:
ghci> prefixes "abc"
["", "a", "ab","abc"]
-}

prefixes' :: [a] -> [[a]]
prefixes' xs = foldr (\x acc -> [] : map (x :) acc) [[]] xs

{-
[]
[] : ["c"]
[] : ["b"] : ["bc"]
[] : ["a"] : ["ab"] : ["abc"]
-}
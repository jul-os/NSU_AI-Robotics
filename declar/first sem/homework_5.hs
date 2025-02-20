or' :: [Bool] -> Bool
or' = foldr (||) False

-- returns True if all items in the list fulfill the condition 
all' :: (Foldable t) => (a -> Bool) -> t a -> Bool
all' p = foldr (\x acc -> p x && acc) True

map' :: (a -> b) -> [a] -> [b]
map' p = foldr (\x acc -> p x : acc) []

concatMap' :: (a -> [b]) -> [a] -> [b]
concatMap' p = foldr (\x acc -> p x ++ acc) []

filter' :: (a -> Bool) -> [a] -> [a]
filter' p = foldr (\x acc -> if p x == True then [x] ++ acc else acc) []

partition' :: (a -> Bool) -> [a] -> ([a], [a])
partition' p = foldr (\x (truth, false) -> if p x then (x : truth, false) else (truth, x : false)) ([], [])

unzip' :: [(a, b)] -> ([a], [b])
unzip' = foldr (\(a, b) (as, bs) -> (a : as, b : bs)) ([], [])

null' :: [a] -> Bool
null' = foldr (\_ _ -> False) True

-- первое _ это текущий элемент а второе _ это аккамул

-- inserts separator between the elements of its list argument 
intersperse' :: a -> [a] -> [a]
intersperse' p = init . foldr (\x acc -> [x] ++ [p] ++ acc) []

group' :: (Eq a) => [a] -> [[a]]
group' = foldr f []
  where
    f a [] = [[a]]
    f a ((x : xs) : xss) = if a == x then (a : x : xs) : xss else [a] : ((x : xs) : xss)

-- 2
{-
l_i :: [(Double, Double)] -> Int -> (Double -> Double)
l_i xs i x =  foldl (\acc j -> if i /= j then acc * (x - fst (xs !! j)) / (fst (xs !! i) - fst (xs !! j)) else acc) 1 [0 .. (length xs - 1)]

lagrange :: [(Double, Double)] -> [Double]
lagrange xs x = map ( foldl (\acc i -> acc + (snd (xs !! i) * (l_i xs i x))) 0 [0 .. (length xs - 1)]) (map fst xs)
-}
{-
lagrange [(-1.5, -14.1014), (-0.75, -0.931596), (0,0), (0.75, 0.931596), (1.5, 14.1014)]
[-14.1014,-0.931596,0.0,0.931596,14.1014]
-}
{-
lagrange' :: [(Double, Double)] -> (Double -> Double)
lagrange' [(x, y)] = map (y * l x)
  where l x
-}

lagrange' :: [(Double, Double)] -> (Double -> Double)
--принимает список точек и возвращает функцию которая возьмет х и вычислит значение полинома в точке
lagrange' xs = \x -> foldl (l_i x) 0.0 (zip [0..] xs)
--zip [0..] xs
--например если [(1,2), (3, 4)] = xs
--то будет [(0, (1,2)), (1, (3, 4))]
--каждой точке списка как бы выдали индекс
  where
    l_i x acc (i, (xi, yi)) = acc + yi * foldl (\prod (j, (xj, _)) -> if j /= i then prod * ((x - xj) / (xi - xj)) else prod) 1.0 (zip [0..] xs)
--принимает точку х и накопитель суммы и вот ту штуку которую создал zip
--складывает аккамулятор и Li (x)

--let points = [(1, 1), (2, 4), (3, 9)]
--lagrange' points 1.5
-- >>2.25

-- 3

{-
f = foldr (-) 0
g = foldl (-) 0
-}

{-
1. У f и g одинаковый тип?
foldr (a -> b -> b) -> b -> [a] -> b
foldl (a -> b -> a) -> a -> [b] -> a
но обе функции берут список и возвращают одно число
то есть у обеих тип NUm a => [a] -> a

2. Придумайте список xs такой, что f xs /= g xs
напимер [1,2,3]
f [1, 2, 3] = (1 - (2 - (3 - 0))) = (1 - (2 - 3 )) =(1 - (-1)) = 2
g [1, 2, 3] = (((0 - 1) - 2) - 3) = ((-1 - 2) - 3) = (-3 - 3) = -6

3. Придумайте не пустой список ys такой, что f ys == g ys .
любой список из одного элемента - эт же число
или список из всех 0, такой как [0, 0, 0]
или список вроде [7, -7, 7, -7] (будет 0)

4. Найдите все значения a, b, и c такие, что f [a,b,c] == g [a,b,c]
(a - (b - (c - 0)) = (((0 - a) - b) - c)
(a - (b - c)) = - a - b - c
a - b + c =  -a - b - c
a = -c
любое b
тогда нужно f[-c, b, c] = g[-c, b, c]
-}
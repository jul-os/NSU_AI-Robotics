s1 :: [Int]
s1 = [1, 2, 3]
--значения одного типа + не задано сколько элементов, это список

c :: (Int, Char, Double)
c = (1, '2', 4.5)
--это кортеж

s2 :: [Int]
--s2 = [1, 2]
-- голова : хвост, он сам разеберется на части, и мы потом использовать и то значение и другое
s2 = 1 : (2 : [])


--[x, y, z] = (x : y : z :[])

s = "Wow"
ss = ['W', 'o', 'w']
sss = 'W' : 'o' : 'w' : []
--записи эквивалентны

-- если писать ((:) xy), то мы просто вынесли операцию : вперед - предикатная  запись

-- []:[] - список из одного пустого списка

second_element :: [a] -> a
second_element [] = error "empty list"
second_element (x : y : s) = y

sum' :: [Int] -> Int
sum' [] = 0
sum' (x : xs) = x + sum' xs

replicate' :: Int -> a -> [a]
replicate' 0 a = []
--replicate' 1 a = [a]
--replicate' 2 a = [a, a]
replicate' n a = (a : replicate' (n - 1) a)

lenght' :: [a] -> Int
lenght' [] = 0
lenght' (x : xs) = 1 + lenght' xs
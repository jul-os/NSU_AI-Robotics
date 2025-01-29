plush' :: Int -> (Int -> Int)
plush' n = let f x = x + n in f

plush'' :: Int -> Int -> Int
plush'' n x = x + n 

sum_squares :: Int -> Int -> Int
sum_squares a b 
    | b == a = b^2
    | b < a = sum_squares b a 
    |otherwise = b^2 + sum_squares a (b - 1)


{-
Напишите функцию, которая применяет функцию к
каждому числу в интервале [a,b] и суммирует
результаты.
-}

higher_order_sum :: (Int -> Int) -> Int -> Int -> Int
higher_order_sum f a b 
    | b == a = f b
    | b < a = higher_order_sum f b a --это если пользователь перепутал порядок
    | otherwise = f b + higher_order_sum f a (b - 1) -- идет рекурсия пока b  не станет a 
-- например f(b) = b + 1 - просто прибавление к числу 1-цы

-- (2^) a b - два в степенях


-- f - то как преобразовывать числа, это какая-то функция



--написать функцию прнимающую другую функцию так чтобы пользователь мог задать какую операцию он хочет сделать
--изменится сигнатура
-- добавить функцию которая применится к двум элементам

--
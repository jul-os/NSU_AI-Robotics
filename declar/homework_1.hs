{-
to compile write in Prelude:
    :load homework_1.hs
to run foo_num write in *Main>
    foo_num input
-}

foo_one :: Double -> Double
-- здесь из области определения задано x <= (-4) || x >= 0 
foo_one x = if x <= (-4) || x >= 0 then x^2/(1 + x) else -10000

foo_two :: Double -> Double
foo_two x = if 3*x - x^3 >= 0 then sqrt(3*x - x^3) else -10000

foo_three :: Double -> Double
--natural logarithm is log func
foo_three x = if x^3 - 21 > 0 then log(x^3 - 21) else -10000

foo_four :: Double -> Double
--logBase Floating a => a -> a -> a second arg is base
-- здесь из области определения задано x > 4
foo_four x = if x > 4 then logBase 2 (logBase 3 (logBase 4 x)) else -10000
--g (h (f 1))	g(h(f(1)))

foo_five :: Double -> Double
foo_five x =  if sin(2*x) >= 0 && sin(3*x) >= 0 then sqrt(sin (2*x)) - sqrt(sin (3*x)) else -10000

distance :: Double  -> Double -> Double -> Double -> Double
distance x1 y1 x2 y2 = sqrt(abs(x2 - x1)^2 + abs(y2 - y1)^2)


leap :: Int -> Bool
--using guards (MOOC cource 2.2)
leap x
    | x `mod` 400 == 0 = True
    | x `mod` 100 == 0 = False
    | x `mod` 4 == 0 = True
    | otherwise = False

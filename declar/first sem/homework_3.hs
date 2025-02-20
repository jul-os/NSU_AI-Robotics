combinePredicates :: (Int -> Bool) -> (Int -> Bool) -> (Int -> Bool)
combinePredicates p1 p2 = \x -> p1 x && p2 x

--2

differentiate :: (Double -> Double) -> Double -> Double -> Double
differentiate f x h = (f (x + h) - f x) / h

--3

solver :: (Double -> Double) -> Double -> Double -> Double -> Double
--         ^f                 ^ точность       ^концы отрезка   ^вывод
solver f e a b
  | (f a) * (f b) > 0 = error "the ends of the segment should have different signs"
  -- из псевдокода из википедии, но с поправкой на <= вместо =
  | abs (f a) <= e = a
  | abs (f b) <= e = b
  | (f a) * (f (a + (b - a) / 2)) < 0 = solver f e a (a + (b - a) / 2)
  | otherwise = solver f e (a + (b - a) / 2) b

{-
примеры: f = x^2 - 4
        корни -2 и 2

        f = x^3 - x^2 + 2
        (-10) 10
        один из корней -1
-}
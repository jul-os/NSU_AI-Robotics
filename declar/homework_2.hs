my_reverse :: Integer -> Integer
my_reverse x = read ( reverse ( show x))
--123 -> "123" -> "321" -> 321

--надо было с рекурсией


is_prime :: Integer -> String
is_prime x = if div x x == 2
          then "prime" 
          else "not prime"
          where
            dividers n i | i == 1 = 1 --все числа делятся на 1 и себя, поэтому ждем 2 и не больше
            dividers n i | n mod i == 0 = 1 + dividers n (i - 1)
            dividers n i = dividers n (i - 1)

fibonacci :: Integer -> Integer
fibonacci n = fibonacci' 0 1 n

fibonacci' :: Integer -> Integer -> Integer -> Integer
fibonacci' a b 1 = b
fibonacci' a b n = fibonacci' b (a + b) (n - 1)

-- b i+1 = q * b i

my_sum :: Double -> Double -> Double -> Double
my_sum b i q
  | i == 1 = b
  |otherwise = b * (q ** (i - 1)) + my_sum b (i - 1) q

progression :: Double -> Double -> Maybe Double
progression b q
  | q >= 1 || q <= -1 = error "the series deverges"
  | otherwise = Just (b / (1 - q))


min_n :: Double -> Double -> Double -> Double
min_n b1 q e | q >= 1 || q <= - 1 = error "the series deverges"
min_n b1 q e = min_n' 1 b1
    where
        min_n' n sum_cur | abs (sum_cur - s) < e = n
        min_n' n sum_cur = min_n' (n+1) (sum_cur + b1 * q**n)
        s = b1 / (1 - q)
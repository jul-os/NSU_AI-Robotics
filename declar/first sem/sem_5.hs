
find_three :: [(Int, Int, Int)]
find_three = [(x, y, z) |  z <- [1..], x <- [1..z], y <- [1..z], x * x + y * y == z * z, x < y]
--где <- значит пренадлежит
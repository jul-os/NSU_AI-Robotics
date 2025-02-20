data DayOfWeek = Monday | Tuesday | Wednesday | Thursday | Friday | Saturday | Sunday
  deriving (Eq)

isWeekend :: DayOfWeek -> Bool
isWeekend p
  | p == Sunday || p == Saturday = True
  | otherwise = False

data PointD = PointD Double Double
  deriving (Show)

distanceToOrigin :: PointD -> Double
distanceToOrigin (PointD x y) = sqrt (x ^ 2 + y ^ 2)

data Shape = Circle Double | Rectangle Double Double | Triangle Double Double Double

area :: Shape -> Double
area (Circle r) = pi * r ^ 2
area (Rectangle a b) = a * b
area (Triangle a b c) = sqrt (p * (p - a) * (p - b) * (p - c))
  where
    p = (a + b + c) / 2

perimeter :: Shape -> Double
perimeter (Circle r) = 2 * pi * r
perimeter (Rectangle x y) = 2 * x + 2 * y
perimeter (Triangle a b c) = a + b + c

isSquare :: Shape -> Bool
isSquare (Rectangle x y) = if x == y then True else False
isSquare _ = False

data Point a = Point a a -- параметрический конструктр типа

euclideanDistance :: Point Double -> Point Double -> Double
euclideanDistance (Point x1 y1) (Point x2 y2) = sqrt ((x2 - x1) ^ 2 + (y2 - y1) ^ 2)

manhDistance :: Point Int -> Point Int -> Int
manhDistance (Point x1 y1) (Point x2 y2) = (x2 - x1) + (y2 - y1)

-- хотим чтобы была функция которая сама понимает к какому типу применяться а не писать отдельную для каждого
-- создадим классы типов

-- класс типов
class Distance a where
  distance :: Point a -> Point a -> a

-- представитель класса типов
instance Distance Int where
  distance = manhDistance

instance Distance Double where
  distance = euclideanDistance

-- вызывать как distance (что-то)

instance (Show a) => Show (Point a) where
  show (Point x y) = "{" ++ show x ++ " , " ++ show y ++ "}"

data List a = Nil | Cons a (List a)
  deriving (Show)
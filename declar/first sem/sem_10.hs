import Data.IntMap (fromList)

data List a = Nil | Cons a (List a)
  deriving (Show)

fromList' :: List a -> [a]
fromList' Nil = []
fromList' (Cons x xs) = x : fromList' xs

toList :: [a] -> List a
toList [] = Nil
toList (x : xs) = Cons x (toList xs)

data Nat = Zero | Suc Nat
  deriving (Show)

-- натуральные числа

fromNat :: Nat -> Integer
fromNat Zero = 0
fromNat (Suc n) = fromNat n + 1

-- fromNat (Suc (Suc Zero))

toNat :: Integer -> Nat
toNat 0 = Zero
toNat n = Suc (toNat (n - 1))

addNat :: Nat -> Nat -> Nat
addNat a b = toNat $ fromNat a + fromNat b

multNat :: Nat -> Nat -> Nat
multNat a b = toNat $ fromNat a * fromNat b

facNat :: Nat -> Nat -> Nat
facNat  Zero = toNat 1
facNat a = toNat $ fromNat a * facNat a (toNat (fromNat a - 1))
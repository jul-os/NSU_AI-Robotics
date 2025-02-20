
data Expr
  = Var
  | Num Integer
  | Add Expr Expr
  | Sub Expr Expr

exp1 = Num 4

exp2 = Add (Num 2) (Num 2)

exp3 = Add (Num 2) (Sub (Num 5) (Num 3))

exp4 = Add (Add (Num 1) (Num 2)) (Num 3)

exp5 = Add (Num 1) (Add (Num 2) (Num 3))

exp6 = Add (Sub (Num 55) (Num 8)) (Sub (Num 13) (Num 7))

eval :: Expr -> Integer
eval (Num n) = n
eval (Add ex1 ex2) = (eval ex1) + (eval ex2)
eval (Sub ex1 ex2) = (eval ex1) - (eval ex2)

instance Show Expr where
  show Var = "x"
  show (Num n) = show n
  show (Add ex1 ex2) = "(" ++ show ex1 ++ " + " ++ show ex2 ++ ")"
  show (Sub ex1 ex2) = "(" ++ show ex1 ++ " - " ++ show ex2 ++ ")"

size :: Expr -> Int
size (Num _) = 0
size (Add exp1 exp2) = 1 + size exp1 + size exp2
size (Sub exp1 exp2) = 1 + size exp1 + size exp2

m_eval :: Expr -> Maybe Integer
m_eval (Var) = Nothing
m_eval (Num n) = Just n
m_eval (Add ex1 ex2) =
  case (m_eval ex1, m_eval ex2) of
    (Just n1, Just n2) -> Just (n1 + n2)
    _ -> Nothing
m_eval (Sub ex1 ex2) =
  case (m_eval ex1, m_eval ex2) of
    (Just n1, Just n2) -> Just (n1 - n2)
    _ -> Nothing

exp0 = Num 0

evalVar :: Expr -> Integer -> Integer
evalVar Var p = p
evalVar (Num n) _  = n
evalVar (Add exp1 exp2) p = (evalVar exp1 p) + (evalVar exp2 p)
evalVar (Sub exp1 exp2) p = (evalVar exp1 p) - (evalVar exp2 p)

data Tree a = Empty | Node a (Tree a) (Tree a)
    deriving (Show, Eq)

tree1 = Node 6
    (Node 4 Empty Empty)
    (Node 3
        (Node 2 Empty Empty)
        (Node 5 Empty Empty))

tree2 = Node 3
    (Node 1 Empty (Node 2 Empty Empty))
    (Node 5 (Node 4 Empty Empty) Empty)

tree3 = Node 4
    (Node 2
        (Node 1 (Node 0 Empty Empty) Empty)
        (Node 3 Empty Empty))
    (Node 6
        (Node 5 Empty Empty)
        (Node 7 Empty Empty))
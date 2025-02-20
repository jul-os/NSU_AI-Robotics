data IntExpr
  = Num Int
  | Add IntExpr IntExpr
  | Sub IntExpr IntExpr
  | Mult IntExpr IntExpr
  | Div IntExpr IntExpr

instance Show IntExpr where
  show (Num n) = show n
  show (Add ex1 ex2) = "(" ++ show ex1 ++ " + " ++ show ex2 ++ ")"
  show (Sub ex1 ex2) = "(" ++ show ex1 ++ " - " ++ show ex2 ++ ")"
  show (Mult ex1 ex2) = "(" ++ show ex1 ++ " * " ++ show ex2 ++ ")"
  show (Div ex1 ex2) = "(" ++ show ex1 ++ " / " ++ show ex2 ++ ")"

intEval :: IntExpr -> Int
intEval (Num n) = n
intEval (Add ex1 ex2) = intEval ex1 + intEval ex2
intEval (Sub ex1 ex2) = intEval ex1 - intEval ex2
intEval (Mult ex1 ex2) = intEval ex1 * intEval ex2
intEval (Div ex1 ex2) = intEval ex1 `div` intEval ex2

{-intEval (Add (Num 5) (Num 7))
12-}

data BoolExpr
  = BoolOne Bool
  | Not BoolExpr
  | And BoolExpr BoolExpr
  | Or BoolExpr BoolExpr
  | Eq IntExpr IntExpr
  | Gt IntExpr IntExpr

-- тк здесь может быть не Int а выражение, то создадим отдельный тип
-- IntExpr описан выше

instance Show BoolExpr where
  show (BoolOne b) = if b then "True" else "False"
  show (Not expr) = "not " ++ show expr
  show (And expr1 expr2) = show expr1 ++ " && " ++ show expr2
  show (Or expr1 expr2) = show expr1 ++ " || " ++ show expr2
  show (Eq expr1 expr2) = show expr1 ++ " == " ++ show expr2
  show (Gt expr1 expr2) = show expr1 ++ " > " ++ show expr2

expr1 = BoolOne True

expr2 = Not $ BoolOne True

expr3 = Or (BoolOne True) (BoolOne False)

expr4 = Gt (Add (Num 5) (Num 6)) (Num 5)

boolEval :: BoolExpr -> Bool
boolEval (BoolOne b) = b
boolEval (Not b) = not $ boolEval b
boolEval (And ex1 ex2) = boolEval ex1 && boolEval ex2
boolEval (Or ex1 ex2) = boolEval ex1 || boolEval ex2
boolEval (Eq int1 int2) = intEval int1 == intEval int2
boolEval (Gt int1 int2) = intEval int1 > intEval int2

-- если отключать Show из последнего задания, то нужно вернуть его в deriving
data Bin = End | O Bin | I Bin
  -- deriving (Show, Eq)
  deriving (Eq)

-- прибавляет 1 к числу
inc :: Bin -> Bin
inc End = I End
inc (O b) = I b
inc (I b) = O (inc b)

fromBin :: Bin -> Int
fromBin End = 0
fromBin (O b) = 2 * fromBin b
fromBin (I b) = 1 + 2 * fromBin b

toBin :: Int -> Bin
toBin 0 = O End
toBin n = go n
  where
    go 0 = End
    go m
      | even m = O (go (m `div` 2))
      | otherwise = I (go (m `div` 2))

pls :: Bin -> Bin -> Bin
pls End End = End
pls End b = b
pls a End = a
pls (O a) (O b) = O (pls a b)
pls (O a) (I b) = I (pls a b)
pls (I a) (O b) = I (pls a b)
pls (I a) (I b) = O (pls (inc a) b)

{-pls (I (I (I (I End)))) (I (I (I End)))
O (I (I (O (I End))))-}

mlt :: Bin -> Bin -> Bin
mlt End _ = End
mlt _ End = End
mlt (O a) b = O (mlt a b)
mlt (I a) b = pls b (O (mlt a b))

{-mlt (I (I (O (I (I End))))) (O (O (I (I End))))
O (O (I (O (O (O (I (O (I End))))))))-}

-- вариант 1

instance Show Bin where
  show End = "0"
  show (I End) = "1"
  show (O End) = "0" -- возникал конфликт с toBin, где пытались показать 0
  show bin = showbin bin
    where
      showbin (O End) = ""
      showbin (I End) = "1"
      showbin (I b) = showbin b ++ "1"
      showbin (O b) = showbin b ++ "0"

-- а потом я почитала задания и подумала, что для предыдущих Show не должен работать как бы?
-- нужно ли его отключать для определенных функций и как это делать?
-- при этом если отключать Show, то нужно вернуть его в deriving

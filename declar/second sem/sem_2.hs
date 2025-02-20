
import qualified Data.Map as M
headE :: [a] -> Either String a
headE [] = Left "can't on empty list"
headE (x:xs) = Right x

tailE :: [a] -> Either String [a]
tailE [] = Left "can't on empty list"
tailE (x:xs) = Right xs

sumFirst :: Num a => [a] -> Either String a
sumFirst xs = (+) <$>  headE xs <*> (tailE xs >>= headE)


data Expr = Num Integer |
            Var Name |
            Bin Op Expr Expr |
            Let Name Expr Expr

data Op = Add | Mul | Sub | Div
type Name = String
data ExprErr = DivisionByZero | UnsetVariable Name
    deriving (Show, Eq)

evalE :: Expr -> M.Map Name Integer -> Either ExprErr Integer
evalE (Num n) env = return n
evalE (Var x) env = case M.lookup x env of
        Just n -> Right n
        Nothing -> Left (UnsetVariable x)
evalE (Bin op a b) env = do
        x <- evalE a env
        y <- evalE b env
        case op of
            Div -> if y == 0 then Left DivisionByZero else Right $ x `div` y
            Add -> Right (x + y)
            Mul -> Right (x * y)
            Sub -> Right (x - y)
evalE (Let x e1 e2) env = do
    t <- evalE e1 env
    let env_new = M.insert x t env
    evalE e2 env_new
    



{-
ghci> evalE (Var "x") M.empty
Left (UnsetVariable "x")
-}

import qualified Data.Map as M
import Control.Monad.Trans.Reader
import Control.Monad.Trans.Writer

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
    
evalR :: Expr -> Reader (M.Map Name Integer) Integer
evalR (Num n) = return n
evalR (Var x) = do
    env <- ask
    case M.lookup x env of
        Just n -> return n
        Nothing -> error "not found"

{-
Используя монаду Writer, напишите функцию правой свертки
списка при помощи операции вычитания

в которой рекурсивные вызовы сопровождались бы записью в лог,
так чтобы в результате получалось такое поведение:

ghci> runWriter $ minusLoggedR 0 [1..3]
(2,"(1-(2-(3-0)))")
-}

minusLoggedR :: (Show a, Num a) => a -> [a] -> Writer String a
minusLoggedR acc [] = do
    tell $ show acc  -- Завершаем выражение числом
    return acc
minusLoggedR acc (x:xs) = do
    tell "("          -- Начинаем новую скобку
    tell $ show x     -- Добавляем текущий элемент
    tell "-"          -- Добавляем оператор вычитания
    res <- minusLoggedR acc xs  -- Рекурсивный вызов для хвоста
    tell ")"          -- Закрываем скобку
    return $ x - res  -- Вычисляем результат

minusLoggedR' :: (Show a, Num a) => a -> [a] -> Writer String a
minusLoggedR' acc xs = foldr f (return acc) xs
  where
    f x w = do
        tell "("
        tell $ show x
        tell "-"
        r <- w
        {-
        w имеет тип Writer String a - это вычисление, которое уже накопило какой-то лог и даст результат типа a
        <- "извлекает" значение из монады Writer
        r будет содержать числовой результат вычисления для хвоста списка
        -}
        tell ")"
        return $ x - r

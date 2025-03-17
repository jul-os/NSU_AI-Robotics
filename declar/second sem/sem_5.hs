{-# LANGUAGE BlockArguments #-}
import Control.Monad.Trans.State
import Control.Monad.Trans.Writer
import Control.Monad.Trans.Maybe
import Control.Monad.Trans.Reader
import qualified Data.Map as M
import Control.Monad.Trans.Class
import GHC.Base (undefined)

{-
Реализуйте функцию paren таким образом, чтобы функция parensMatch, проверяющая правильность 
расстановки скобок в строке, работала корректно.
ghci>parensMatch "()"
True
ghci>parensMatch "("
False
ghci>parensMatch "())"
False
ghci>parensMatch "(()(()()))"
True
ghci>parensMatch "(()((()))"
False
ghci>parensMatch "(()))("
False
-}
{-
paren :: Char -> State Int ()
paren c = do
        cnt <- get
        
        case c of
                '(' -> put (cnt + 1)
                ')' -> put (cnt - 1)
                _ -> return () 
-}
    -- get - обратиться к состоянию
    --  <- - достать из контекста
    -- (()))( получился отрицательный счетчик и в итоге программа считала что все верно

paren :: Char -> State Int ()
paren c = do
        cnt <- get
        if cnt < 0
            then return ()
            else case c of
                '(' -> put (cnt + 1)
                ')' -> put (cnt - 1)
                _ -> return () 

    
parensMatch :: String -> Bool
parensMatch s = count == 0
  where (_,count) = runState (mapM_ paren s) 0


{-
Реализуйте функции push, pop и empty для работы со стеком, который реализован с помощью списка, хранящегося в состоянии.
-}
-- state - спиисок элементов типа а
-- сам стек лежит в состоянии
push :: a -> State [a] ()
push el = do
        stack <- get
        put (el : stack)
        return ()
-- добавить в голову

pop :: State [a] a
pop = do
    stack <- get
    let hd = head stack
    put (tail stack)
    return hd
-- взять голову

isEmpty :: State [a] Bool
isEmpty = do
        stack <- get
        let is = null stack
        return is
-- проверка пустой список или нет


stack :: State [Int] (Int,Int,Int)
stack = do
  push 42
  push 43
  push 44
  x <- pop
  y <- pop
  z <- pop
  return (x,y,z)

r = evalState stack []

{-
Напишите функцию, которая 
1) печатает значение из состояния с приветствием, 
2) увеличивает его на 1 
3) возвращает исходное значение, преобразованное в строку:
ghci> runStateT sPrintIncAccum 10
Hello, 10
("10",11)
-}
sPrintIncAccum :: (Num a, Show a) => StateT a IO String
sPrintIncAccum = do
        n <- get
        lift $ putStrLn ("Hello, " ++ show n)
        put (n + 1)
        return (show n)



data Expr = Num Integer | 
            Var Name |
            Bin Op Expr Expr | 
            Let Name Expr Expr
data Op = Add | Mul | Sub | Div
          
type Name = String

instance Show Op where
    show Add = " + "
    show Mul = " * "
    show Sub = " - "
    show Div = " / "

instance Show Expr where
    show (Num n) = show n
    show (Var x) = x
    show (Bin op e1 e2) = "(" ++ show e1 ++ show op ++ show e2 ++ ")"
    show (Let x e1 e2) = "(let " ++ x ++ " = " ++ show e1 ++ " in " ++ show e2 ++ ")"

eop :: Op -> Integer -> Integer -> Integer
eop Div = div
eop Mul = (*)
eop Add = (+)
eop Sub = (-)

name :: Op -> String
name Div = "div"
name Add = "add"
name Sub = "sub"
name Mul = "mul"

{-
Для выражений напишите функцию eval, которая
- вычисляет значение выражения, если это возможно (поскольку окружение не задано, в выражении не должно быть переменных)
- логгирует вычисления

Чем отличаются варианты 11 и 12?

ghci> e = (Bin Add (Bin Add (Num 2) (Num 3)) (Num 1))
ghci> e
((2 + 3) + 1)
ghci> runWriterT (eval11 e)
Just (6,"add 2 3;add 5 1;")
ghci> runWriter $ runMaybeT (eval12 e)
(Just 6,"add 2 3;add 5 1;")
ghci> runWriterT (eval11 v)
Nothing
ghci> runWriter $ runMaybeT (eval12 v)
(Nothing,"add 2 3;var x is not defined")
-}
-- wont work if for example ... x1 but x1 in not defined 
-- wont work if division by 0
eval11 :: Expr -> WriterT String Maybe Integer
eval11 (Num n) = return n 
eval11 (Var x) = lift Nothing
eval11 ( Bin op e1 e2) = do
    x <- eval11 e1 
    y <- eval11 e2
    tell $ name op ++ " " ++ show x ++ " " ++ show y ++ ";"
    case op of
        Div -> if y == 0 then lift Nothing else return (div x y)
        _ -> return (eop op x y)
        -- в результате maybe снаружи поэтому если где-то будет ошибка просто выведет nothing
        {-newtype WriterT w m a =
WriterT { runWriterT :: m (a, w) }-}
        
-- сделаем наоборот чтобы можно было посмореть внутренний лог ошибки
{-
newtype MaybeT m a =
MaybeT { runMaybeT :: m (Maybe a) }
-}

--MaybeT автоматически оборачивает значение в Just.
eval12 :: Expr -> MaybeT (Writer String) Integer
eval12 (Num n) = return n
eval12 (Var x) = do
    lift $ tell ("variable not defined")
    MaybeT (return Nothing)
eval12 (Bin op e1 e2) = do
    x <- eval12 e1
    y <- eval12 e2
    lift $ tell (name op ++ " " ++ show x ++ " " ++ show y ++ ";")
    case op of
        Div -> if y == 0 
               then MaybeT (return Nothing) 
               else return (div x y)
        _ -> return (eop op x y)
---
{-
Напишите функцию eval, которая вычисляет значение выражения в **окружении**, **если это возможно**. 
Используйте `Reader/ReaderT` и `Maybe/MaybeT`

-}
type Env = M.Map Name Integer
eval2 :: Expr -> ReaderT Env Maybe Integer
eval2 (Num n) = return n
eval2 (Var x) = do
    env <- ask
    case M.lookup x env of
        Just value -> return value
        Nothing -> lift Nothing
eval2 (Bin op e1 e2) = do
    x <- eval2 e1
    y <- eval2 e2
    case op of
        Div -> if y == 0 then lift Nothing else return (div x y)
        _ -> return (eop op x y)
eval2 (Let x e1 e2) = do
    env <- ask  
    maybeValue <- eval2 e1  
    case maybeValue of
        Nothing -> lift Nothing  -- Если `e1` вернуло `Nothing`, значит ошибка
        Just value -> 
            let newEnv = M.insert x value env  
            in local (const newEnv) (eval2 e2)


{-
Добавьте логгирование к предыдущей функции.
-}

eval3 = undefined
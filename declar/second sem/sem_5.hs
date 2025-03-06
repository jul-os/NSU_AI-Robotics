import Control.Monad.Trans.State
import Control.Monad.Trans.Writer
import Control.Monad.Trans.Maybe
import qualified Data.Map as M
import Control.Monad.Trans.Class

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


eval11 :: Expr -> WriterT String Maybe Integer
eval11 _ = undefined

eval12 :: Expr -> MaybeT (Writer String) Integer
eval12 _ = undefined

---
{-
Напишите функцию eval, которая вычисляет значение выражения в **окружении**, **если это возможно**. 
Используйте `Reader/ReaderT` и `Maybe/MaybeT`

-}
type Env = M.Map Name Integer
eval2 = undefined

{-
Добавьте логгирование к предыдущей функции.
-}

eval3 = undefined
import Control.Monad.Trans.Class
import Control.Monad.Trans.State
import Control.Monad.Trans.Writer
import Data.Array
import Data.Graph

data Color = White | Gray | Black deriving (Eq, Show)

type Coloring = Array Vertex Color

gr :: Graph
gr = buildG (0, 6) [(0, 1), (0, 6), (1, 2), (6, 5), (6, 2), (6, 4), (5, 1), (4, 2), (3, 6), (2, 3)]

defaultColoring :: Array Int Color
defaultColoring = array (0, 6) [(i, White) | i <- [0 .. 6]]

{-
Напишите функцию обхода графа в глубину, используя монаду StateT и монаду Writer.
Функция должна возвращать True, если в процессе обхода был найден цикл, и False в противном случае.
Выводите в лог информацию о посещенных вершинах.
Лог должен иметь следующий формат (для графа gr):
"Visiting node 0;Visiting node 6;Visiting node 4;Visiting node 2;Visiting node 3;
Again visiting 6! Found cycle!;Left node 3;Left node 2;Left node 4;Visiting node 5;
Visiting node 1;Left node 1;Left node 5;Left node 6;Left node 0;"
-}


deepFirst :: Graph -> Vertex -> StateT Coloring (Writer String) Bool
deepFirst g v = do
  coloring <- get
  case coloring ! v of
    White -> do
      modify (// [(v, Gray)])
      lift $ tell ("Visiting node " ++ show v ++ ";")
      results <- mapM (\w -> deepFirst g w) (g ! v)
      modify (// [(v, Black)])
      lift $ tell ("Left node " ++ show v ++ ";")
      return (or results)
    Gray -> do
      lift $ tell ("Again visiting node " ++ show v ++ "; Found cycle!")
      return True
    Black -> return False

{-
Используя предыдущую функцию, проверьте есть ли в графе циклы
-}
hasCycles :: Graph -> Bool
hasCycles g = fst $ runWriter $ evalStateT (anyM (deepFirst g) (indices g)) initialColoring
  where
    initialColoring = array (bounds g) [(v, White) | v <- indices g]

anyM :: (Monad m) => (a -> m Bool) -> [a] -> m Bool
anyM _ [] = return False
anyM f (x:xs) = do
  res <- f x
  if res then return True else anyM f xs

{-
deepFirst записывает сообщения (Writer String), но нам нужно только Bool, поэтому берем fst от результата runWriter.

bounds g даёт диапазон индексов вершин
indices g даёт список всех вершин графа.
-}


{-
Напишите функцию, которая строит список вершин
графа в порядке обхода в ширину (начиная с
заданной)
-}
breadthFirst :: Graph -> Vertex -> [Vertex]
breadthFirst g v = undefined

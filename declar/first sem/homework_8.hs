import Data.List (sort)

data Tree a = Empty | Node a (Tree a) (Tree a)
  deriving (Read, Eq)

isSymmetric :: Tree a -> Bool
isSymmetric Empty = True
isSymmetric (Node _ left right) = areMirrored left right

areMirrored :: Tree a -> Tree a -> Bool
areMirrored Empty Empty = True
areMirrored (Node _ l1 r1) (Node _ l2 r2) = areMirrored l1 r2 && areMirrored r1 l2
areMirrored _ _ = False

tree2 :: Tree Int
tree2 =
  Node
    4
    ( Node
        2
        (Node 1 Empty Empty)
        (Node 3 Empty Empty)
    )
    ( Node
        6
        (Node 5 Empty Empty)
        (Node 7 Empty Empty)
    )

buildBalancedBST :: (Ord a) => [a] -> Tree a
buildBalancedBST [] = Empty
buildBalancedBST xs =
  let sorted = sort (removeDuplicates xs)
      mid = length sorted `div` 2
      (left, (x : right)) = splitAt mid sorted -- чтобы как бы выделить части для следующей строки
   in Node x (buildBalancedBST left) (buildBalancedBST right)

removeDuplicates :: (Eq a) => [a] -> [a]
removeDuplicates = foldr (\x acc -> if x `elem` acc then acc else x : acc) []

list = [1, 2, 3, 4, 5, 10, 5, 11, 12, 7899]

tree1 = buildBalancedBST list

deleteMin :: (Ord a) => Tree a -> (Tree a, a)
deleteMin Empty = error "Tree is empty"
deleteMin (Node x Empty right) = (right, x)
deleteMin (Node x left right) =
  let (newLeft, minVal) = deleteMin left
   in (Node x newLeft right, minVal)

deleteValue :: (Ord a) => a -> Tree a -> Tree a
deleteValue _ Empty = Empty
deleteValue x (Node v left right)
  | x < v = Node v (deleteValue x left) right
  | x > v = Node v left (deleteValue x right)
  | otherwise = deleteNode v left right

deleteNode :: (Ord a) => a -> Tree a -> Tree a -> Tree a
deleteNode _ Empty right = right
deleteNode _ left Empty = left
deleteNode _ left right =
  let (rightSubTree, minVal) = deleteMin right
   in Node minVal left rightSubTree

height :: Tree a -> Int
height Empty = 0
height (Node x Empty Empty) = 0
height (Node _ left right) = 1 + max (height left) (height right)

-- Функция для вывода значения головного узла
printRoot :: (Show a) => Tree a -> String
printRoot Empty = "" -- Если дерево пустое, выводим сообщение
printRoot (Node x _ _) = show x

instance (Show a) => Show (Tree a) where
  show t = drawTree 0 t
    where
      drawTree _ Empty = "" -- Represent Empty nodes as "[]"
      drawTree 0 (Node x l r) =
        -- самый первый узел печатает свою голову
        replicate ((height (Node x l r) * 2) + height (Node x l r)) ' '
          ++ show x
          ++ "\n"
          ++ replicate ((height (Node x l r) - 1) * 2) ' '
          ++ printRoot l
          ++ replicate (((height (Node x l r)) * 3)) ' '
          ++ printRoot r
          ++ "\n"
          ++ drawTree 1 l
          ++ drawTree 2 r
      drawTree 1 (Node x l r) =
        replicate ((height (Node x l r) - 1) * 2) ' '
          ++ printRoot l
          ++ replicate (((height (Node x l r)) * 3)) ' '
          ++ printRoot r
          --  ++ "\n"
          ++ drawTree 1 l
          ++ " "
          ++ drawTree 2 r
      drawTree 2 (Node x l r) =
        printRoot l
          ++ replicate (((height (Node x l r)) * 3)) ' '
          ++ printRoot r
          --  ++ "\n"
          ++ drawTree 1 l
          ++ " "
          ++ drawTree 2 r

{-
я хотела распечатать вертикально вроде
  2
1   3
но так плохо работает
-}

-- вариант 2

{-
instance (Show a) => Show (Tree a) where
  show t = showTree 0 t
    where
      showTree i Empty = (replicate i ' ') ++ "_" ++ "\n"
      showTree i (Node n Empty Empty) = (replicate i ' ') ++ show n ++ "\n"
      showTree i (Node n a b) = (replicate i ' ') ++ show n ++ "\n" ++ (showTree (i + 4) a) ++ (showTree (i + 4) b)
-}
tree3 = buildBalancedBST [1, 2, 3]

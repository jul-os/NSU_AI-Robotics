import Control.Applicative (Alternative (empty))
import Distribution.Compat.Graph (Node (N))

data Tree a = Empty | Node a (Tree a) (Tree a)
  deriving (Show, Eq)

tree1 =
  Node
    6
    (Node 4 Empty Empty)
    ( Node
        3
        (Node 2 Empty Empty)
        (Node 5 Empty Empty)
    )

valAtRoot :: Tree a -> Maybe a
valAtRoot Empty = Nothing
valAtRoot (Node x _ _) = Just x

treeSize :: Tree a -> Int
treeSize Empty = 0
treeSize (Node x l r) = 1 + treeSize l + treeSize r

treeSum :: (Num a) => Tree a -> a
treeSum Empty = 0
treeSum (Node x l r) = x + treeSum l + treeSum r

mapTree :: (a -> b) -> Tree a -> Tree b
mapTree _ Empty = Empty
mapTree f (Node x l r) = Node (f x) (mapTree f l) (mapTree f r)

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

preorder :: Tree a -> [a]
preorder Empty = []
preorder (Node x l r) = [x] ++ preorder l ++ preorder r

inorder :: Tree a -> [a]
inorder Empty = []
inorder (Node x l r) = inorder l ++ [x] ++ inorder r

postorder :: Tree a -> [a]
postorder Empty = []
postorder (Node x l r) = postorder l ++ postorder r ++ [x]

treeInsert :: (Ord a) => a -> Tree a -> Tree a
treeInsert x Empty = Node x Empty Empty
treeInsert x (Node val l r)
  | x < val = Node val (treeInsert x l) r
  | x > val = Node val l (treeInsert x r)
  | x == val = Node val l r
{-
Напишите функцию, которая считает
частоту каждого элемента в списке: 
-}
{-
frequencies :: Eq a => [a] -> [(a,Int)]
frequencies list = foldl f (0, head list) list
        where f :: Eq a => (Int, a) -> (Int, a)
                if a 

-}
import Data.List (group, sort)
import Test.Hspec

frequencies :: (Eq a, Ord a) => [a] -> [(a, Int)]
frequencies list = map (\g -> (head g, length g)) (group (sort list))

main :: IO ()
main = hspec $ do
        describe "frequencies" $ do
                it "groups elements of a list with its frequency" $ do
                        frequencies ([]) `shouldBe` ([] :: [(Int, Int)])

                it "groups elements of a list with its frequency" $ do
                        frequencies [0, 0, 1] `shouldBe` [(0, 2), (1, 1)]
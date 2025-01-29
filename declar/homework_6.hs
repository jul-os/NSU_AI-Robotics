import Data.Char (chr, isLower, isUpper, ord, toLower, toUpper)
import Data.List (nub, permutations, sort)
import Distribution.Simple.Command (OptDescr (BoolOpt))
import Text.PrettyPrint (Style (lineLength))

caesarEncrypt :: String -> Int -> String
caesarEncrypt line shift = map (shift_char shift) line
  where
    shift_char p ch
      | isLower ch = chr (ord 'a' + ((ord ch - ord 'a' + p) `mod` 26))
      | isUpper ch = chr (ord 'A' + ((ord ch - ord 'A' + p) `mod` 26))
      | otherwise = ch

caesarDecrypt :: String -> Int -> String
caesarDecrypt line shift = map (shift_back_char shift) line
  where
    shift_back_char shift ch
      | isLower ch = chr (ord 'a' + ((ord ch - ord 'a' - shift) `mod` 26))
      | isUpper ch = chr (ord 'A' + ((ord ch - ord 'A' - shift) `mod` 26))
      | otherwise = ch

caesarCrack :: String -> [String]
caesarCrack line = [caesarEncrypt line x | x <- [1 .. 25]]

-- 2.2
isVowel :: Char -> Bool
isVowel ch = contains (ord ch) [65, 69, 73, 79, 85, 89, 97, 101, 105, 111, 117, 121]
  where
    -- строчные и прописные 'a', 'e', 'i', 'o', 'u', 'y'

    contains elem myList = case myList of
      [] -> False
      (x : xs) | x == elem -> True
      (_ : xs) -> contains elem xs

delete_vowels :: String -> String
delete_vowels = filter (not . isVowel)

-- 2.1
-- уникальность букв

unique' :: (Eq a) => [a] -> Bool
unique' [] = True
unique' [_] = True
unique' (x : xs)
  | x `elem` xs = False
  | otherwise = unique' xs

-- 2.4 префиксы

prefix_for_two :: String -> String -> String
prefix_for_two [] [] = []
prefix_for_two x [] = []
prefix_for_two [] x = []
prefix_for_two (x : xs) (y : ys)
  | x == y = x : prefix_for_two xs ys
  | x /= y = []

prefix :: [String] -> String
prefix [] = []
prefix [x] = x
prefix (x : xs) = foldl prefix_for_two x xs

-- тут х - начальное значение, xs - список
-- x - первая строка в поданном списке, xs - все остальные строки

-- 2.7
mySplit :: Char -> String -> [String]
mySplit p [] = []
mySplit p line = takeWhile (/= p) line : mySplit p (dropWhile (== p) (dropWhile (/= p) line))

word' :: String -> [String]
word' [] = []
word' line = takeWhile (/= ' ') line : word' (dropWhile (== ' ') (dropWhile (/= ' ') line))

-- 2.3 анаграмма или нет

is_anagram :: String -> String -> Bool
is_anagram x y = if (map toLower x) == (map toLower y) then False else sort (filter (/= ' ') (map toLower x)) == sort (filter (/= ' ') (map toLower y))

-- 2.6
anagramsFor :: String -> [String] -> [String]
anagramsFor line list = filter (\x -> is_anagram line x && x /= line) list

-- 2.5 найти все анаграммы
find_anagrams :: String -> [String]
find_anagrams line = nub [perm | perm <- permutations line, is_anagram line perm]
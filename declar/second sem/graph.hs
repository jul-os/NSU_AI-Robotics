module Graph where

import Text.ParserCombinators.Parsec
import Control.Monad (void, when)
import Data.Char 
import Data.List

data Graph = Digraph ID [Node] [Edge] [Attr] 
  deriving (Show)
data Node = Node ID [Attr] 
  deriving (Show)
data Edge = Edge ID ID [Attr] 
  deriving (Show)
data Attr = Attr ID ID 
  deriving (Show, Eq, Ord)
type ID = String


graph :: Parser Graph
graph = do
  void $ string "digraph"
  skipSpaces
  name <- many1 (noneOf " {;\t\n\r")
  skipSpaces
  void $ char '{'
  skipSpaces
  (nodes, edges, attrs) <- statements
  skipSpaces
  void $ char '}'
  eof
  return (Digraph name (nubBy nodeEq nodes) edges attrs)
  where
    nodeEq (Node id1 _) (Node id2 _) = id1 == id2


statements :: Parser ([Node], [Edge], [Attr])
statements = do
  stmts <- many (try statement <* skipSpaces)
  let (ns, es, as) = unzip3 stmts
  return (concat ns, concat es, concat as)

statement :: Parser ([Node], [Edge], [Attr])
statement = do
  res <- try attrStatement
        <|> try edgeChainStatement
        <|> try nodeStatement
  optional (char ';')
  skipSpaces
  return res


nodeStatement :: Parser ([Node], [Edge], [Attr])
nodeStatement = do
  id <- nodeId
  attrs <- optionMaybe (try (skipSpaces >> attrList))
  case attrs of
    Just a -> return ([Node id a], [], [])
    Nothing -> return ([], [], [])  


edgeChainStatement :: Parser ([Node], [Edge], [Attr])
edgeChainStatement = do
  first <- nodeId
  skipSpaces
  -- Проверяем, что дальше идет ребро, а не атрибут
  lookAhead (try (string "->"))
  void $ string "->"
  skipSpaces
  second <- nodeId
  rest <- many (try $ do
    skipSpaces
    void $ string "->"
    skipSpaces
    nodeId)
  skipSpaces
  attrs <- option [] (try attrList)
  let allNodes = first : second : rest
      edges = zipWith (\f t -> Edge f t attrs) allNodes (tail allNodes)
      nodes = map (\id -> Node id []) allNodes
  return (nodes, edges, [])


attrStatement :: Parser ([Node], [Edge], [Attr])
attrStatement = do
  key <- many1 (noneOf " \t\n\r=;{}[]")
  skipSpaces
  void $ char '='
  skipSpaces
  val <- many1 (noneOf " \t\n\r;}]")
  return ([], [], [Attr key val])


attrList :: Parser [Attr]
attrList = between (char '[' >> skipSpaces) (skipSpaces >> char ']') $
           attribute `sepBy` (skipSpaces >> oneOf ",;" >> skipSpaces)


attribute :: Parser Attr
attribute = do
  key <- many1 (noneOf "= \t\n\r,;]")
  skipSpaces
  void $ char '='
  skipSpaces
  val <- many1 (noneOf " \t\n\r,;]")
  return (Attr key val)


nodeId :: Parser ID
nodeId = do
  first <- satisfy (\c -> isAlpha c || c == '_')
  rest <- many (satisfy (\c -> isAlphaNum c || c == '_'))
  return (first:rest)
  where
    satisfy p = try $ do
      c <- anyChar
      if p c then return c else fail ""

-- Пропуск пробелов
skipSpaces :: Parser ()
skipSpaces = void $ many (oneOf " \t\n\r")


main :: IO ()
main = do
  text <- readFile "graph.txt"
  case parse (graph <* eof) "graph.txt" text of
    Left err -> putStrLn $ "Ошибка: " ++ show err
    Right g -> do
      putStrLn "Read graph:"
      print g
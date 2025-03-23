module WolfsAndSheep where
import Control.Monad.IO.Class 
import Control.Monad
import Control.Monad.Trans.Except
import Control.Monad.Trans.Writer
import Control.Monad.Except
import Control.Monad.Writer
import Control.Monad.Trans.Class (lift)
import System.Random
import Control.Monad.Identity
import Data.Char
import Data.Maybe (fromMaybe)
import Data.List (find, nub)
import Debug.Trace (trace)



-- Положение на доске
data Position = Position {f :: Char, r :: Int}
  deriving (Eq)

instance Show Position where
  show :: Position -> String
  show (Position f r) = f : show r

-- Один ход
data Step = Step {from :: Position, to :: Position}
  deriving (Eq)

instance Show Step where
  show (Step f t) = show f ++ " -> " ++ show t

data Game = Game {sheep :: Position, wolfs :: [Position]}
  deriving (Eq)

posToCoords :: Position -> (Int, Int)
posToCoords (Position file rank) = (fromEnum file - fromEnum 'a' + 1, rank)

coordsToPos :: (Int, Int) -> Position
coordsToPos (x, y) = Position (toEnum (x + fromEnum 'a' - 1)) y

drawBoard :: Game -> String
drawBoard game = unlines [row y | y <- [8, 7 .. 1]]
  where
    (sx, sy) = posToCoords (sheep game)
    wolfPositions = map posToCoords (wolfs game)
    row y = [cell x y | x <- [1 .. 8]]
    cell x y
      | odd (x + y) = ' '
      | (x, y) == (sx, sy) = 'S'
      | (x, y) `elem` wolfPositions = 'W'
      | otherwise = '.'

instance Show Game where
  show game = "Board:\n" ++ drawBoard game

data GameResult = SheepWin | WolfsWin
  deriving (Show)

startSheep :: Position
startSheep = Position 'a' 1

startWolfs :: [Position]
startWolfs = [Position f 8 | f <- "bdfh"]
        --[Position 'a' 3, Position 'b' 4, Position 'd' 2, Position 'c' 1]

startGame :: Game
startGame = Game startSheep startWolfs

possibleSheepSteps :: Game -> [Step]
possibleSheepSteps game =
  [ Step {from = sheepPos, to = coordsToPos (x, y)}
    | let sheepPos = sheep game,
      let (xs, ys) = posToCoords (sheep game),
      y <- [1 .. 8],
      x <- [1 .. 8],
      abs (x - xs) == 1,
      abs (y - ys) == 1,
      even (x + y),
      coordsToPos (x, y) `notElem` wolfs game
  ]

doPossibleSheepSteps :: Game -> [Step]
doPossibleSheepSteps game = do
  sheepPos <- [sheep game] -- Оборачиваем овцу в список, чтобы использовать монаду списка. Так мы делаем его доступным для перебора.
  let (xs, ys) = posToCoords sheepPos
  x <- [1 .. 8]
  y <- [1 .. 8]
  guard (abs (x - xs) == 1 && abs (y - ys) == 1 && even (x + y) && coordsToPos (x, y) `notElem` wolfs game)
  return Step {from = sheepPos, to = coordsToPos (x, y)}

possibleWolfsSteps :: Game -> [Step]
possibleWolfsSteps game =
  [ Step {from = wolfPos, to = coordsToPos (x, y)}
    | wolfPos <- wolfs game,
      let (xs, ys) = posToCoords wolfPos,
      y <- [1 .. 8],
      x <- [1 .. 8],
      abs (xs - x) == 1,
      y == ys - 1,
      even (x + y),
      --coordsToPos (x, y) /= sheep game 
      --я три часа искала почему волки не побеждают а оказалось им надо смотреть что они не наступают на овцу только после хода овцы
      coordsToPos (x, y) `notElem` wolfs game
  ]

doPossibleWolfsSteps :: Game -> [Step]
doPossibleWolfsSteps game = do
        wolfPos <- wolfs game --а тут оно и без нас список видимо и все норм
        let (xs, ys) = posToCoords wolfPos
        let y = ys - 1
        x <- [1..8]
        guard ( abs (x-xs) == 1 && even (x + y)  && coordsToPos (x, y) `notElem` wolfs game && y>= 1 && y <= 8)
        return Step {from = wolfPos, to = coordsToPos (x, y)}

-- Если у игрока несколько доступных ходов, выбирайте первый из доступных.
-- сначала ходит овца

simpleTurn :: Game -> Either GameResult Game
simpleTurn game = 
        let 
                sheepTurn = possibleSheepSteps game
                wolvesTurn = possibleWolfsSteps game
        in case (sheepTurn, wolvesTurn) of
                ([], _) -> Left WolfsWin
                (_, []) -> Left SheepWin
                _ -> Right (updateGame game sheepTurn wolvesTurn)


updateGame :: Game -> [Step] -> [Step] -> Game
updateGame game sheepTurn wolvesTurn =
  let newSheepPos = to (head sheepTurn)  -- Овца делает первый доступный ход
      chosenWolfStep = find (\s -> from s `elem` wolfs game && to s /= newSheepPos) wolvesTurn  -- Находим первый ход для любого волка
      newWolvesPos = case chosenWolfStep of
        Just step -> [if w == from step then to step else w | w <- wolfs game]  -- Один волк ходит, остальные остаются
        Nothing -> wolfs game  -- Если нет ходов, все волки остаются на месте
  in game {sheep = newSheepPos, wolfs = newWolvesPos}

run :: Monad m => (a -> m a) -> a -> m a
run f x = (run f) =<< (f x)

loggedTurn :: Game -> WriterT [Step] (Either GameResult) Game
loggedTurn (Game sheep wolves) = do
    let sheeps = possibleSheepSteps (Game sheep wolves)
    case sheeps of
        [] -> do
            Control.Monad.Writer.tell []
            lift (Left WolfsWin)
        (Step _ (Position sx sy)): _ -> do
            Control.Monad.Writer.tell [head sheeps]
            if sy == 8
                then do
                    Control.Monad.Writer.tell []
                    lift (Left SheepWin)
                else do
                    -- Проверяем волков
                    let wolvess = possibleWolfsSteps (Game (Position sx sy) wolves)
                    case wolvess of
                        [] -> do
                            Control.Monad.Writer.tell []
                            lift (Left SheepWin)
                        (Step (Position wx wy) (Position wxn wyn)): _ -> do
                            Control.Monad.Writer.tell [head wolvess]
                            lift (Right (Game (Position sx sy) ((Position wxn wyn) : filter (/= (Position wx wy)) wolves)))


loggedTurn' :: Game -> ExceptT GameResult (Writer [Step]) Game
loggedTurn' (Game sheep wolves) = do
        let sheeps = possibleSheepSteps (Game sheep wolves)
        case sheeps of
                [] -> do
                        lift (Control.Monad.Writer.tell [])
                        throwE WolfsWin
                (Step _ (Position sx sy)): _ -> do
                        lift (Control.Monad.Writer.tell [head sheeps])
                        if sy == 8
                        then do
                                lift (Control.Monad.Writer.tell [])
                                throwE SheepWin
                        else do
                                -- Проверяем волков
                                let wolvess = possibleWolfsSteps (Game (Position sx sy) wolves)
                                case wolvess of
                                        [] -> do
                                                lift (Control.Monad.Writer.tell [])
                                                throwE SheepWin
                                        (Step (Position wx wy) (Position wxn wyn)): _ -> do
                                                lift (Control.Monad.Writer.tell [head wolvess])
                                                return (Game (Position sx sy) ((Position wxn wyn) : filter (/= (Position wx wy)) wolves))


randomElement :: [a] -> IO (Maybe a)
randomElement [] = return Nothing
randomElement xs = do
    idx <- randomRIO (0, length xs - 1)  
    return (Just (xs !! idx))

newtype MyMonad a = MyMonad { runMyMonad :: ExceptT GameResult (WriterT [Step] IO) a }
    deriving (Functor, Applicative, Monad, MonadIO, MonadError GameResult, MonadWriter [Step])


randomGame :: Game -> MyMonad Game
randomGame game = do
    liftIO (putStrLn (show game))
    let sheeps = possibleSheepSteps game
    case sheeps of
        [] -> do
            liftIO (putStrLn "No possible steps for the sheep!")
            throwError WolfsWin 
        _ -> do
            sheepStep <- liftIO (randomElement sheeps)
            case sheepStep of
                Nothing -> do
                    liftIO (putStrLn "No random step for the sheep!")
                    throwError WolfsWin
                Just (Step _ (Position sx sy)) -> do
                    Control.Monad.Writer.tell [head sheeps]
                    if sy == 8
                    then do
                        liftIO (putStrLn "Sheep reached the end!")
                        throwError SheepWin
                    else do
                        let wolvess = possibleWolfsSteps (Game (Position sx sy) (wolfs game))
                        case wolvess of
                            [] -> do
                                liftIO (putStrLn "No possible steps for the wolves!")
                                throwError SheepWin
                            _ -> do
                                wolfStep <- liftIO (randomElement wolvess)
                                case wolfStep of
                                    Nothing -> do
                                        liftIO (putStrLn "No random step for the wolves!")
                                        throwError SheepWin
                                    Just (Step (Position wf wr) (Position wfn wrn)) -> do
                                        Control.Monad.Writer.tell [Step (Position wf wr) (Position wfn wrn)]  
                                        -- рекурсия потому что я устала думать и не хочу писать состояния
                                        randomGame (Game (Position sx sy) ((Position wfn wrn) : filter (/= (Position wf wr)) (wolfs game)))


runRandGame :: Game -> IO ()
runRandGame game = do
    result <- runWriterT (runExceptT (runMyMonad (randomGame game))) 
    let (finalGame, logSteps) = result  
    putStrLn "Game result:"
    print finalGame  
    putStrLn "Log of steps:"
    mapM_ (putStrLn . show) logSteps

--runRandGame startGame
showPossibleSteps :: Int -> [Step] -> String
showPossibleSteps _ [] = ""
showPossibleSteps n (x:xs) = show n ++ ". " ++ show x ++ "\n" ++ showPossibleSteps (n + 1) xs

gameWithUser :: Game -> MyMonad Game
gameWithUser (Game sheep wolves) = do
    let sheeps = possibleSheepSteps (Game sheep wolves)
    case sheeps of
        [] -> do
            liftIO (putStrLn "No possible steps for the sheep!")
            throwError WolfsWin  
        _ -> do
            liftIO (putStrLn (show (Game sheep wolves))) 
            liftIO (putStrLn "Possible steps for the sheep:") 
            liftIO (putStrLn (showPossibleSteps 1 sheeps))
            temp <- liftIO getLine
            let index = if read temp > length sheeps then 0 else read temp - 1
            let input = sheeps !! index
            Control.Monad.Writer.tell [input]
            let (Step _ (Position sx sy)) = input
            if sy == 8
            then do
                liftIO (putStrLn "Sheep reached the end!")
                throwError SheepWin  
            else do
                let wolvess = possibleWolfsSteps (Game (Position sx sy) wolves)
                case wolvess of
                    [] -> do
                        liftIO (putStrLn "No possible steps for the wolves!")
                        throwError SheepWin  
                    _ -> do
                        wolfStep <- liftIO (randomElement wolvess)
                        case wolfStep of
                            Nothing -> do
                                liftIO (putStrLn "No random step for the wolves!")
                                throwError SheepWin
                            Just (Step (Position wf wr) (Position wfn wrn)) -> do
                                Control.Monad.Writer.tell [Step (Position wf wr) (Position wfn wrn)]
                                        -- рекурсия потому что я устала думать и не хочу писать состояния
                                gameWithUser (Game (Position sx sy) ((Position wfn wrn) : filter (/= (Position wf wr)) wolves))

runGameWithUser :: Game -> IO ()
runGameWithUser game = do
    result <- runWriterT (runExceptT (runMyMonad (gameWithUser game)))  
    let (finalGame, logSteps) = result  
    putStrLn "Game result:"
    print finalGame  
    putStrLn "Log of steps:"
    mapM_ (putStrLn . show) logSteps

-- Ура, теперь можно играть! Волки не очень умные, но им и не надо, они рандомные
-- вам приятного вечера или дня, а мне спокойной ночи :)
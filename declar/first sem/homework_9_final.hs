import Data.Map (Map)
import Data.Map qualified as Map
import Text.Regex.Posix ((=~))
import System.IO (writeFile, readFile)
import Data.Char (isDigit)
import System.Directory (doesFileExist)
--если опять не запускается run with :set -package directory (in ghci)

while :: IO Bool -> IO () -> IO ()
while cond op = do
  a <- cond
  if a
    then do
      op
      while cond op
    else
      return ()


checkPhoneNumber :: String -> Bool
checkPhoneNumber input = input =~ format7 || input =~ format8 || input =~ format71 || input =~ format81
  where
    format7 = "^[+]7[ -]?[0-9]{3}[ -]?[0-9]{3}[ -]?[0-9]{4}$"
    format71 = "^[+]7[ -]?[(][0-9]{3}[)][ -]?[0-9]{3}[ -]?[0-9]{4}$"
    format8 = "^8[ -]?[0-9]{3}[ -]?[0-9]{3}[ -]?[0-9]{4}$"
    format81 = "^8[ -]?[(][0-9]{3}[)][ -]?[0-9]{3}[ -]?[0-9]{4}$"

formatPhoneNumber :: String -> String
formatPhoneNumber input = 
    let digits = drop 1 $ filter isDigit input
        firstThree = take 3 digits
        secondThree = take 3 (drop 3 digits)
        last = drop 6 digits
    in "+7-" ++ firstThree ++ "-" ++ secondThree ++ "-" ++ last

type PhoneBook = Map String String
{-
caseOne :: String -> PhoneBook -> IO PhoneBook
caseOne input thisBook = do
  let (name, rawPhoneNumber) = break (== ' ') input
  let phoneNumber = dropWhile (== ' ') rawPhoneNumber
  if not (checkPhoneNumber phoneNumber)
    then do
          let cond = do
          if checkPhoneNumber phoneNumber
            then return False
            else do 
              putStrLn "Please, write the number in a correct format.Try again, and write the number only\n"
              phoneNumber <- getLine
              return (not (checkPhoneNumber newPhoneNumber))

    else do
      let formattedNumber = formatPhoneNumber phoneNumber
      if Map.member name thisBook
        then do
          putStrLn "A contact with this name already exists. Are you sure you want to rewrite it? [Y/N]\n"
          reply <- getLine
          if reply == "Y"
            then do
              putStrLn "overwriting..."
              let newthisBook = Map.insert name formattedNumber thisBook
              return newthisBook
            else do
              putStrLn "not changing..."
              return thisBook
        else do
          putStrLn "adding..."
          let newthisBook = Map.insert name formattedNumber thisBook
          return newthisBook
-}

getCorrectPhoneNumber number = do
        if checkPhoneNumber number
          then return number 
          else do
            putStrLn "Please, write the number in a correct format. Try again, and write the number only:"
            newNumber <- getLine
            getCorrectPhoneNumber newNumber 

caseOne :: String -> PhoneBook -> IO PhoneBook
caseOne input thisBook = do
  let (name, rawPhoneNumber) = break (== ' ') input
  let phoneNumber = dropWhile (== ' ') rawPhoneNumber

  checkedPhoneNumber <- if checkPhoneNumber phoneNumber
                        then return phoneNumber
                        else getCorrectPhoneNumber phoneNumber

  let formattedNumber = formatPhoneNumber checkedPhoneNumber
  if Map.member name thisBook
    then do
      putStrLn "A contact with this name already exists. Are you sure you want to rewrite it? [Y/N]"
      reply <- getLine
      if reply == "Y"
        then do
          putStrLn "Overwriting..."
          let newThisBook = Map.insert name formattedNumber thisBook
          return newThisBook
        else do
          putStrLn "Not changing..."
          return thisBook
    else do
      putStrLn "Adding..."
      let newThisBook = Map.insert name formattedNumber thisBook
      return newThisBook


caseTwo :: String -> PhoneBook -> IO PhoneBook
caseTwo name phones = do
  case Map.lookup name phones of
    Just phone -> do
      let formattedPhone = formatPhoneNumber phone
      putStrLn $ "Phone number for " ++ name ++ ": " ++ formattedPhone
    Nothing -> putStrLn "Contact not found."
  return phones


caseThree :: String -> PhoneBook -> IO PhoneBook
caseThree name phones = do
  if Map.member name phones
    then do
      let updatedPhones = Map.delete name phones
      putStrLn $ "Contact " ++ name ++ " deleted."
      return updatedPhones
    else do
      putStrLn "Contact not found."
      return phones

saveToFile :: FilePath -> PhoneBook -> IO ()
saveToFile filePath phoneBook = do
  let content = unlines $ map (\(name, phone) -> name ++ " " ++ phone) (Map.toList phoneBook)
  writeFile filePath content
  putStrLn $ "Phone book saved to " ++ filePath


loadFromFile :: FilePath -> IO PhoneBook
loadFromFile fileName = do
  fileExists <- doesFileExist fileName
  if not fileExists
    then do
      putStrLn $ "Error: File \"" ++ fileName ++ "\" does not exist."
      return Map.empty
    else do
      content <- readFile fileName
      let entries = lines content
      let updatedBook = foldl (\book line ->
                                let (name, rawPhone) = break (== ' ') line
                                    phone = dropWhile (== ' ') rawPhone
                                    formattedPhone = formatPhoneNumber phone
                                in Map.insert name formattedPhone book) Map.empty entries
      putStrLn "Phone book successfully loaded and formatted."
      return updatedBook


choiceAction :: PhoneBook -> IO (Bool, PhoneBook)
choiceAction thisBook = do
  putStrLn "Hello! What do you want to do?\n    1. Add a contact\n    2. Find a contact\n    3. Delete a contact\n    4. Save phone book\n    5. Load phone book\n    6. Quit\nPlease, enter a corresponding number\n"
  choice <- getLine
  case choice of
    "1" -> do
      putStrLn "Please, write name and phone number:\n"
      answer <- getLine
      newthisBook <- caseOne answer thisBook
      return (True, newthisBook)
    "2" -> do
      putStrLn "Please, write the name of the contact you want to find\n"
      answer <- getLine
      newthisBook <- caseTwo answer thisBook
      return (True, newthisBook)
    "3" -> do
      putStrLn "Please, write the name of the contact you want to delete\n"
      answer <- getLine
      newthisBook <- caseThree answer thisBook
      return (True, newthisBook)
    "4" -> do
      putStrLn "Please, enter the filename to save the phone book:\n"
      fileName <- getLine
      saveToFile fileName thisBook
      return (True, thisBook)
    "5" -> do
      putStrLn "Please, enter the filename to load the phone book from:\n"
      fileName <- getLine
      newthisBook <- loadFromFile fileName
      return (True, newthisBook)
    "6" -> do
      putStrLn "Goodbye!\n"
      return (False, thisBook)
    _ -> do
      putStrLn "Invalid input. Try again.\n"
      return (True, thisBook)

main :: IO ()
main = do
  let thisBook = Map.empty :: PhoneBook
  loop thisBook
  where
    loop :: PhoneBook -> IO ()
    loop thisBook = do
      (continue, newthisBook) <- choiceAction thisBook
      if continue
        then loop newthisBook
        else return ()

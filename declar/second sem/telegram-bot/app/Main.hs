module Main where

import Connection (getUpdates)

data Model = Model 

data Action = DoNothing


main :: IO ()
main = getUpdates
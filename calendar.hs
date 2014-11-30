import Data.Time
import Data.List

import System.Locale
import Data.Time
import Data.Time.Format
import Data.Time.Calendar.WeekDate


--------------------
-- Date Time helper
--------------------

getYear :: Day -> Integer
getYear d = y where (y, _, _) = toGregorian d

equalYear :: Day -> Day -> Bool
equalYear d1 d2 = (getYear d1) == (getYear d2)

getMonth :: Day -> Int
getMonth d = m where (_, m, _) = toGregorian d

equalMonth :: Day -> Day -> Bool
equalMonth d1 d2 = (getMonth d1) == (getMonth d2)

getDay :: Day -> Int
getDay d = dd where (_, _, dd) = toGregorian d

equalDay :: Day -> Day -> Bool
equalDay d1 d2 = (getDay d1) == (getDay d2)

dayOfWeek :: Day -> Int
dayOfWeek d = dw where (_, _, dw) = toWeekDate d

weekOfYear :: Day -> Int
weekOfYear d = wy where (_, wy, _) = toWeekDate d

equalWeek :: Day -> Day -> Bool
equalWeek d1 d2 =  (weekOfYear d1) == (weekOfYear d2)

---------------------
-- Calendar Formating
---------------------

colsPerDay = 3;
 
-- The number of columns per week in the formatted output.
colsPerWeek = 7 * colsPerDay;

spaces :: Int -> String
spaces n = replicate n ' '

formatWeek :: [Day] -> String
formatWeek ds =    spaces (colsPerDay * startDay)
                ++ unwords (map (formatTime defaultTimeLocale "%d") ds)
                ++ spaces (colsPerDay * ( 7 - startDay - length ds))
                where startDay = dayOfWeek (head ds) - 1

monthsString = ["January", "February", "March", "April", "May", "June",
                "July", "August", "September", "October", "November", "December"]

monthTitle :: Int -> String
monthTitle n = (spaces before) ++ name ++ (spaces after)
               where name   = monthsString !! (n - 1)
                     before = (colsPerWeek - length name) `div` 2
                     after  = (colsPerWeek - length name - before)


formatMonth :: [Day] -> String
formatMonth ds = (monthTitle $ getMonth $ head ds) ++ "\n" ++
                 (intercalate "\n" (map formatWeek (groupBy equalWeek ds)))

formatMonths :: [Day] -> String
formatMonths ms = intercalate "\n\n" (map formatMonth (groupBy equalMonth ms))

datesInYear :: Integer -> [Day]
datesInYear y = [x | x <- iterate (addDays 1) (fromGregorian y 1 1),
                 getYear x == y]

main :: IO ()
main = do putStr $ formatMonths $ datesInYear 2014

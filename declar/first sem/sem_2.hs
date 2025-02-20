fact_double :: Integer -> Integer
fact_double n | n < 0 = error "Negative n!"
fact_double 0 = 1
fact_double 1 = 1
fact_double n = n * fact_double (n - 2)

ackermann :: Integer -> Integer -> Integer
ackermann m n
    | m == 0 = n + 1
    | m > 0 && n == 0 = ackermann(m - 1) 1
    | m > 0 && n > 0 = ackermann (m - 1) (ackermann m (n-1))
    --                 g          h    (   f          1   ))
    --                             g h (f 1) = g(h, f(1))


-- in prelude :r = reload. use when the file is modified

fibb :: Integer -> Integer
fibb 0 = 0
fibb 1 = 1
fibb n = fibb(n - 2) + fibb (n - 1)
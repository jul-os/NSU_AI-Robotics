// структура, в которой хранится разложение на простые множители
typedef struct Factors
{
    int k;          // сколько различных простых в разложении
    int primes[32]; // различные простые в порядке возрастания
    int powers[32]; // в какие степени надо эти простые возводить
} Factors;
// функция, которая находит разложение числа X и записывает его в структуру res

//в худшем случае перебираем sqrt(x) чисел и делим х на это все за O(logx)
// =>  O(sqrt(x))
void Factorize(int X, Factors *res){
    int deviderCounter = 0;
    int currentDevider = 2;

    while (X != 1 && currentDevider * currentDevider <= X) { // ищем все простые до корня из числа
        if (X % currentDevider == 0) { // если currentDevider является деолителем,
                                         // то запишем его в делители и в счетчик запишем 0
            res->primes[deviderCounter] = currentDevider;
            res->powers[deviderCounter] = 0;

            while (X % currentDevider == 0) { // пока число делится на курдив, мы делим его и инкрементим счетчик
                res->powers[deviderCounter]++;
                X /= currentDevider;
            }
            deviderCounter++; // увеличиваем счетчик делителей
        }
        currentDevider++; // ищем следующий делитель
    }

    if (X != 1) { // случай, когда после деления осталось простое число
        res->primes[deviderCounter] = X;
        res->powers[deviderCounter] = 1;printf("1 = 1\n");
        return;
        deviderCounter++;
    }
    res->k = deviderCounter; // счетчик различных делителей
}
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>

static const long Num_To_Add = 1000000000;
static const double Scale = 10.0 / RAND_MAX;

long add_parallel(const char*);

long add_serial(const char* numbers);

int main() {
    srand(time(NULL));
    char* numbers = new char[(sizeof(char) * Num_To_Add)];

    long chunk_size = Num_To_Add / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int)time(NULL) + (unsigned int)p;
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++) {
            numbers[i] = (char)(rand() * Scale);
        }
    }

    clock_t start, end;

    printf("Timing sequential...\n");
    start = clock();

    long sum_s = add_serial(numbers);

    end = clock();
    printf("Took %f seconds\n\n", (double)(end - start) / CLOCKS_PER_SEC);

    printf("Timing parallel...\n");
    start = clock();

    long sum_p = add_parallel(numbers);

    end = clock();
    printf("Took %f seconds\n\n", (double) (end - start) / CLOCKS_PER_SEC);

    printf("Sum serial: %ld\nSum parallel: %ld", sum_s, sum_p);

    free(numbers);
    return 0;
}

long add_parallel(const char* numbers) {
    long sum = 0;
# pragma omp parallel for num_threads(omp_get_max_threads()) reduction(+:sum)
    for (long i = 0; i < Num_To_Add; i++) {
        sum += numbers[i];
    }
    return sum;
}

long add_serial(const char* numbers) {
    long sum = 0;
    for (long i = 0; i < Num_To_Add; i++) {
        sum += numbers[i];
    }
    return sum;
}
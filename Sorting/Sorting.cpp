#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <memory.h>
#include <iostream>

using namespace std;

static const long Num_To_Sort = 100000;


void mergeSort_s(int arr[], int const begin, int const end);
void merge(int array[], int const left, int const mid, int const right);
void mergeSort_p(int arr[], int const begin, int const end);

int main() {
    clock_t start, end;
    int* arr_s = new int[Num_To_Sort];
    int* arr_p = new int[Num_To_Sort];
    long chunk_size = Num_To_Sort / omp_get_max_threads();
    srand(time(NULL));

#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++) {
            arr_s[i] = arr_p[i] = rand();
        }
    }

    // Copy the array so that the sorting function can operate on it directly.
    // Note that this doubles the memory usage.
    // You may wish to test with slightly smaller arrays if you're running out of memory.

    printf("Timing sequential...\n");
    start = clock();

    mergeSort_s(arr_s, 0, Num_To_Sort - 1);

    end = clock();
    printf("Took %f seconds\n\n", (double)(end - start) / CLOCKS_PER_SEC);

    printf("Timing parallel...\n");
    start = clock();

    mergeSort_p(arr_p, 0, Num_To_Sort - 1);

    end = clock();
    printf("Took %f seconds\n\n", (double)(end - start) / CLOCKS_PER_SEC);

    printf("sequential: %d, %d, %d, %d, %d \n", arr_s[0], arr_s[1], arr_s[Num_To_Sort / 2], arr_s[Num_To_Sort - 2], arr_s[Num_To_Sort - 1]);
    printf("parallel: %d, %d, %d, %d, %d \n", arr_p[0], arr_p[1], arr_p[Num_To_Sort / 2], arr_p[Num_To_Sort - 2], arr_p[Num_To_Sort - 1]);

    free(arr_s);
    free(arr_p);

    return 0;
}

// https://www.geeksforgeeks.org/merge-sort/

// Sequential version of your sort
// If you're implementing the PSRS algorithm, you may ignore this section
void mergeSort_s(int arr[], int const begin, int const end) {
    if (begin >= end)
        return; // Returns recursively

    auto mid = (begin + end) / 2;
    mergeSort_s(arr, begin, mid);
    mergeSort_s(arr, mid + 1, end);
    merge(arr, begin, mid, end);
}

void merge(int array[], int const low, int const mid, int const high)
{
    int temp[Num_To_Sort];
    int i, j, k, m;
    j = low;
    m = mid + 1;

    for (i = low; j <= mid && m <= high; i++) {
        if (array[j] <= array[m]) {
            temp[i] = array[j];
            j++;
        }
        else {
            temp[i] = array[m];
            m++;
        }
    }

    if (j > mid) {
        for (k = m; k <= high; k++) {
            temp[i] = array[k];
            i++;
        }
    }
    else {
        for (k = j; k <= mid; k++) {
            temp[i] = array[k];
            i++;
        }
    }

    for (k = low; k <= high; k++)
        array[k] = temp[k];
}

// Parallel version of your sort
void mergeSort_p(int arr[], int const begin, int const end) {
    if (begin >= end)
        return; // Returns recursively

    auto mid = (begin + end) / 2;
#pragma omp parallel sections num_threads(2)
    {
        #pragma omp section
        {
            mergeSort_p(arr, begin, mid);
        }
        #pragma omp section
        {
            mergeSort_p(arr, mid + 1, end);
        }
    }
    merge(arr, begin, mid, end);
}
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <iostream>
#include "omp.h"

using namespace std;

int lenArr = 262144000; // 262144000 = 1000 MBytes = 1 GByte
int numthreads = 8;

void quickSort_parallel(int *array, int lenArray, int numThreads);

void quickSort_parallel_internal(int *array, int left, int right, int cutoff);

void quickSort(int *arr, int left, int right) {
    int i = left, j = right;
    int tmp;
    int pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j) {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    if (left < j) {
        quickSort(arr, left, j);
    }
    if (i < right) {
        quickSort(arr, i, right);
    }
}

int main() {
    int maxNumbersDisplayed = 100;

    double startTime, stopTime;

    int *arr1 = new int[lenArr];
    int *arr2 = new int[lenArr];

    int i;
    srand((unsigned) time(NULL));
    printf("Initializing array with random numbers\n");
    for (i = 0; i < lenArr; i++) {
        arr1[i] = rand() % 1000;
        arr2[i] = arr1[i];
    }
    printf("Initialization complete\n");

    if (lenArr <= maxNumbersDisplayed) {
        printf("\n\nArray before sorting: \n");
        for (i = 0; i < lenArr; i++) {
            printf("%d ", arr1[i]);
        }
        printf("\n");
    }

    printf("\nSorting with QuickSort");
    fflush(stdout);
    startTime = clock();
    quickSort(arr1, 0, lenArr - 1);
    stopTime = clock();

    if (lenArr <= maxNumbersDisplayed) {
        for (i = 0; i < lenArr; i++) {
            printf("%d ", arr1[i]);
        }
    }
    printf("\nSorted in (aprox.): %f seconds \n\n", (double) (stopTime - startTime) / CLOCKS_PER_SEC);

    printf("\nSorting with parallel QuickSort");
    fflush(stdout);
    startTime = omp_get_wtime();
    quickSort_parallel(arr2, lenArr, numthreads);
    stopTime = omp_get_wtime();

    if (lenArr <= maxNumbersDisplayed) {
        for (i = 0; i < lenArr; i++) {
            printf("%d ", arr2[i]);
        }
    }
    printf("\nSorted in (aprox.): %f seconds \n", (stopTime - startTime));

    return 0;
}

void quickSort_parallel(int *array, int lenArray, int numThreads) {

    int cutoff = 100000000;

#pragma omp parallel num_threads(1)
    {
#pragma omp single nowait
        {
            quickSort_parallel_internal(array, 0, lenArray - 1, cutoff);
        }
    }

}

void quickSort_parallel_internal(int *array, int left, int right, int cutoff) {
    int i = left, j = right;
    int tmp;
    int pivot = array[(left + right) / 2];

    while (i <= j) {
        while (array[i] < pivot)
            i++;
        while (array[j] > pivot)
            j--;
        if (i <= j) {
            tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            i++;
            j--;
        }
    }

    if (((right - left) < cutoff)) {
        if (left < j) {
            quickSort_parallel_internal(array, left, j, cutoff);
        }
        if (i < right) {
            quickSort_parallel_internal(array, i, right, cutoff);
        }
    } else {
#pragma omp task
        {
            quickSort_parallel_internal(array, left, j, cutoff);
        }
#pragma omp task
        {
            quickSort_parallel_internal(array, i, right, cutoff);
        }
    }
}
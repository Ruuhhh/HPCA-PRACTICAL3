#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define THRESHOLD 1000

// Merge two sorted halves
void merge(int *arr, int start, int mid, int end) {
    int *temp = (int *)malloc((end - start) * sizeof(int));
    int i = start, j = mid, k = 0;

    while (i < mid && j < end) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }
    while (i < mid) temp[k++] = arr[i++];
    while (j < end) temp[k++] = arr[j++];

    for (i = start, k = 0; i < end; i++, k++)
        arr[i] = temp[k];

    free(temp);
}

// Recursive parallel merge sort
void parallel_merge_sort(int *arr, int start, int end) {
    if (end - start <= THRESHOLD) {
        // Insertion sort for small arrays
        for (int i = start + 1; i < end; i++) {
            int key = arr[i];
            int j = i - 1;
            while (j >= start && arr[j] > key)
                arr[j + 1] = arr[j--];
            arr[j + 1] = key;
        }
        return;
    }

    int mid = (start + end) / 2;

    #pragma omp task
    parallel_merge_sort(arr, start, mid);

    #pragma omp task
    parallel_merge_sort(arr, mid, end);

    #pragma omp taskwait
    merge(arr, start, mid, end);
}

int main() {
    int n = 1000000;
    int *arr = (int *)malloc(n * sizeof(int));

    // Initialize array with random values
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 100000;

    double start_time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        parallel_merge_sort(arr, 0, n);
    }

    double end_time = omp_get_wtime();
    printf("Sorting completed in %f seconds\n", end_time - start_time);

    // Check if sorted
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            printf("Array not sorted properly.\n");
            break;
        }
    }

    free(arr);
    return 0;
}

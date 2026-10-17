/*
 * week4_1_dynamic_array.c
 * Author: [salim anrifoudine]
 * Student ID: [260ADB184]
 * Description:
 *   Demonstrates creation and usage of a dynamic array using malloc.
 *   Allocate memory for n integers, read them from the user,
 *   print their sum and average, and then free the memory.
 *
 *   Output must match the format in the Week 4 instructions exactly
 *   (it is checked by the autograder).
 */


#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    printf("Enter number of elements: ");
    // Validate the entered size (must be a number greater than 0)
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid size.\n");
        return 1;
    }

    // Dynamic memory allocation for the integer array
    int *arr = malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Enter %d integers: ", n);
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        // Verification of Each User Input
        if (scanf("%d", &arr[i]) != 1) {
            printf("Invalid input.\n");
            free(arr); // Libération de la mémoire avant de quitter en cas d'erreur
            return 1;
        }
        sum += arr[i];
    }

    // Calculating the Average Using Floating-Point Arithmetic
    double average = (double)sum / n;

    // Displaying results in the requested format
    printf("Sum = %lld\n", sum);
    printf("Average = %.2f\n", average);

    // Freeing Allocated Memory
    free(arr);
    return 0;
}

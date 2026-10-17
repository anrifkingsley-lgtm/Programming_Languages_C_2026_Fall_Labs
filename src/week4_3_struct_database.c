/*
 * week4_3_struct_database.c
 * Author: [salim anrifoudine]
 * Student ID: [260ADB184]
 * Description:
 *   Simple in-memory "database" using an array of structs.
 *   Use malloc to allocate space for n Student records,
 *   read each record from the user, print them as a table,
 *   and then free the memory.
 *
 *   Output must match the format in the Week 4 instructions exactly
 *   (it is checked by the autograder).
 */
/*
 * Name: Salim Morchoudine
 * Student ID: VOTRE_NUMERO_ETUDIANT
 */

#include <stdio.h>
#include <stdlib.h>

// Define the Student structure
struct Student {
    char name[50];
    int id;
    float grade;
};

int main(void) {
    int n;
    printf("Enter number of students: ");
    
    // Check if the entered number of students is valid (> 0)
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number.\n");
        return 1;
    }

    // Dynamically allocate memory for an array of Student records using malloc
    struct Student *students = malloc(n * sizeof(struct Student));
    
    // Check if malloc returned a valid pointer
    if (students == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Read data for each student from the user
    for (int i = 0; i < n; i++) {
        printf("Enter data for student %d: ", i + 1);
        if (scanf("%49s %d %f", students[i].name, &students[i].id, &students[i].grade) != 3) {
            printf("Invalid input.\n");
            free(students); // Free the allocated memory before exiting on error
            return 1;
        }
    }

    // Print the table header and rows in the correct format
    printf("\n");
    printf("%-6s %-11s %s\n", "ID", "Name", "Grade");
    for (int i = 0; i < n; i++) {
        printf("%-6d %-11s %.1f\n", students[i].id, students[i].name, students[i].grade);
    }

    // Free all allocated memory before program exit
    free(students);
    return 0;
}

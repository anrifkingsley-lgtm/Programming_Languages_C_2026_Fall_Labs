/*
 * week4_2_struct_student.c
 * Author: [salim anrifoudine]
 * Student ID: [260ADB184]
 * Description:
 *   Demonstrates defining and using a struct in C.
 *   Define a 'Student' struct with name, id and grade, create two
 *   instances with the values from the instructions, and print them.
 *
 *   This program reads no input. Output must match the format in the
 *   Week 4 instructions exactly (it is checked by the autograder).
 */

#include <stdio.h>
#include <string.h>

// TODO: Define struct Student with fields: name (char[50]), id (int), grade (float)
// Example:
// struct Student {
//     char name[50];
//     int id;
//     float grade;
// };


// Define the Student structure
struct Student {
    char name[50];
    int id;
    float grade;
};

int main(void) {
    struct Student s1, s2;

    // Assign data for the first student
    strcpy(s1.name, "Alice Johnson");
    s1.id = 1001;
    s1.grade = 9.1f;

    // Assign data for the second student
    strcpy(s2.name, "Bob Smith");
    s2.id = 1002;
    s2.grade = 8.7f;

    // Print information in the exact required format
    printf("Student 1: %s, ID: %d, Grade: %.1f\n", s1.name, s1.id, s1.grade);
    printf("Student 2: %s, ID: %d, Grade: %.1f\n", s2.name, s2.id, s2.grade);

    return 0;
}
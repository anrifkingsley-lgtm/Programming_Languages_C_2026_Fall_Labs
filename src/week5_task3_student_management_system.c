// week5_task3_student_management_system.c
// Name: __anrifoudine______  Surname: ___salim_____  Student ID: _260ADB184_______
//
// Task 3: Mini-project – Student management system with file persistence
// Week 5 – Files & Modular Programming
// TODO: Implement the functions to load, save, add, and list students.
//
// Build: make lab5   Run: ./bin/week5_task3_student_management_system

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define NAME_LEN 50
#define LINE_LEN 256
#define DATA_FILE "students.txt"

typedef struct Student {
    char name[NAME_LEN];
    int id;
    float gpa;
} Student;

// Function prototypes (do not change these signatures – the autograder calls them directly)
int  load_students(Student arr[], int max);          // returns number of records loaded
int  save_students(const Student arr[], int count);  // 0 on success, -1 on error
void add_student(Student arr[], int *count);
void list_students(const Student arr[], int count);
int  read_line(const char *prompt, char *buf, size_t size);

int main(void) {
    Student students[MAX_STUDENTS];
    int count = 0;
    char input[LINE_LEN];
    int running = 1;

    //  Load existing data: count = load_students(students, MAX_STUDENTS);
    count = load_students(students, MAX_STUDENTS);
    printf("Loaded %d student(s) from %s\n", count, DATA_FILE);

    while (running) {
        printf("\n=== Student Management System ===\n");
        printf("1. List students\n");
        printf("2. Add student\n");
        printf("3. Save and Exit\n");

        int choice;
        if (read_line("Select an option: ", input, sizeof input) != 0) {
            choice = 3;                    // end of input: save and exit instead of looping forever
        } else {
            char *end;
            long value = strtol(input, &end, 10);
            choice = (end != input && *end == '\0') ? (int)value : 0;   // non-numbers become invalid
        }

        switch (choice) {
            case 1:
                //  Call list_students()
                list_students(students, count);
                break;
            case 2:
                //  Call add_student()
                add_student(students, &count);
                break;
            case 3:
                //  Call save_students(). If it fails, print an error message.
                if (save_students(students, count) != 0) {
                    perror("Error saving students");
                }
                printf("Data saved. Goodbye!\n");
                running = 0;
                break;
            default:
                printf("Invalid option. Try again.\n");
        }
    }

    return EXIT_SUCCESS;
}


int read_line(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, (int)size, stdin) == NULL) {
        return -1;
    }
    size_t len = strcspn(buf, "\r\n");
    if (buf[len] == '\0' && len == size - 1) {
        
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    buf[len] = '\0';
    return 0;
}


int load_students(Student arr[], int max) {
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    char line[LINE_LEN];
    int count = 0;
    while (fgets(line, sizeof(line), fp) != NULL && count < max) {
        char name[NAME_LEN];
        int id;
        float gpa;
        
        if (sscanf(line, "%49[^,],%d,%f", name, &id, &gpa) == 3) {
            strcpy(arr[count].name, name);
            arr[count].id = id;
            arr[count].gpa = gpa;
            count++;
        }
    }

    fclose(fp);
    return count;
}


// Open DATA_FILE for writing ("w"), return -1 if fopen() fails.
// Write one line per student: fprintf(fp, "%s,%d,%.2f\n", ...). Close the file and return 0.
int save_students(const Student arr[], int count) {
    FILE *fp = fopen(DATA_FILE, "w");
    if (fp == NULL) {
        return -1;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%d,%.2f\n", arr[i].name, arr[i].id, arr[i].gpa);
    }

    fclose(fp);
    return 0;
}


void add_student(Student arr[], int *count) {
    if (*count >= MAX_STUDENTS) {
        printf("Cannot add more students (limit reached).\n");
        return;
    }

    char name_buf[LINE_LEN];
    char id_buf[LINE_LEN];
    char gpa_buf[LINE_LEN];
    char *endptr;

    // 2. Read Name
    if (read_line("Enter name: ", name_buf, sizeof(name_buf)) != 0) {
        return;
    }
    // 3. Validate Name (not empty, no comma)
    if (strlen(name_buf) == 0 || strchr(name_buf, ',') != NULL) {
        printf("Invalid input. Student not added.\n");
        return;
    }

    // Read ID
    if (read_line("Enter ID: ", id_buf, sizeof(id_buf)) != 0) {
        return;
    }
    // Validate ID (integer conversion)
    long id_val = strtol(id_buf, &endptr, 10);
    if (endptr == id_buf || *endptr != '\0') {
        printf("Invalid input. Student not added.\n");
        return;
    }

    // Read GPA
    if (read_line("Enter GPA: ", gpa_buf, sizeof(gpa_buf)) != 0) {
        return;
    }
    
    float gpa_val = strtof(gpa_buf, &endptr);
    if (endptr == gpa_buf || *endptr != '\0' || gpa_val < 0.0f || gpa_val > 4.0f) {
        printf("Invalid input. Student not added.\n");
        return;
    }

    // 4. Store student data
    strcpy(arr[*count].name, name_buf);
    arr[*count].id = (int)id_val;
    arr[*count].gpa = gpa_val;
    (*count)++;

    printf("Student added successfully!\n");
}


// If count is 0 print "No students found."; otherwise print a header and one line per student,
// e.g. printf("%-5d %-20s %.2f\n", id, name, gpa);
void list_students(const Student arr[], int count) {
    if (count == 0) {
        printf("No students found.\n");
        return;
    }

    printf("%-6s %-11s %s\n", "ID", "Name", "GPA");
    for (int i = 0; i < count; i++) {
        printf("%-6d %-11s %.2f\n", arr[i].id, arr[i].name, arr[i].gpa);
    }
}
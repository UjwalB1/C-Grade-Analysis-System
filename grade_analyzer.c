#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// max sizes
#define COMP_NAME_SIZE 50
#define STUDENT_NAME_SIZE 50

// student structure for storing mult student records
typedef struct {
    char name[STUDENT_NAME_SIZE];
    double finalGrade;
    char letterGrade[3];
} Student;

// fn prototypes
void componentScoreInputOption1(int n, char compNames[][COMP_NAME_SIZE], double weight[], double maxPoints[], double scores[]);
void componentInputOption2(int n, char compNames[][COMP_NAME_SIZE], double weight[], double maxPoints[]);
void scoreInputOption2(int n, char compNames[][COMP_NAME_SIZE], double maxPoints[], double scores[]);
double computeGradePercentage(int n, double weight[], double maxPoints[], double scores[]);
void grade(int finalGrade, char letterGrade[]);
void calculateStatistics(int n, double grades[], double *max, double *avg, double *median);
void sortGrades(double grades[], int n);

// additional helper fns
void optionSingleStudent(void);
void optionMultipleStudents(void);
void optionSortStudents(void);
void optionSearchStudent(void);
int compareStudentsDesc(const void *a, const void *b);
void clearInputBuffer(void);

// global dynamic array for student records from Option 2 n cuz they also get used by option 3 n 4
Student *students = NULL;
int numStudents = 0;

int main(void)
{
    while (true)
    {
        int choice = 0;
        puts("======================================");
        puts("Weighted Grade Calculation Program");
        puts("======================================");
        puts("1. Single Student Grades");
        puts("2. Multiple Students Grades");
        puts("3. Sort Students by Grade");
        puts("4. Search for a Student");
        puts("5. Exit");
        
        while (choice <= 0)
        {
            printf("Enter your choice (1/2/3/4/5): ");
            if (scanf("%d", &choice) != 1) {
                clearInputBuffer();
                choice = 0;
                continue;
            }
        }
        clearInputBuffer(); // clear leftover newline
        
        if (choice == 1)
            optionSingleStudent();
        else if (choice == 2)
            optionMultipleStudents();
        else if (choice == 3)
            optionSortStudents();
        else if (choice == 4)
            optionSearchStudent();
        else if (choice == 5)
        {
            puts("--- HAVE A GOOD DAY! ---");
            puts("========================");
            break;
        }
        else
            printf("Invalid option. Please try again.\n");
    }
    
    if (students != NULL)
        free(students);
    return 0;
}

// clear input buf
void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// single Student grade calc
void optionSingleStudent(void)
{
    char studentName[STUDENT_NAME_SIZE];
    char letterGrade[3] = " ";
    int n = 0;
    
    // prompt for students full name
    printf("\nEnter student's full name: ");
    fgets(studentName, STUDENT_NAME_SIZE, stdin);
    studentName[strcspn(studentName, "\n")] = '\0';
    
    // prompt for num of course components
    while (n <= 0)
    {
        printf("Enter number of course components: ");
        if (scanf("%d", &n) != 1)
        {
            clearInputBuffer();
            n = 0;
            continue;
        }
    }
    clearInputBuffer();
    
    // arrs for component details n scores
    char compNames[n][COMP_NAME_SIZE];
    double weight[n], maxPoints[n], scores[n];
    
    // use a combined fn to input component info n earned points
    componentScoreInputOption1(n, compNames, weight, maxPoints, scores);
    
    int finalGrade = (int) computeGradePercentage(n, weight, maxPoints, scores);
    printf("\n-- Summary of Scores --\n");
    printf("Final Weighted Grade: %.0f%%\n", round(finalGrade));
    grade(finalGrade, letterGrade);
    printf("Letter Grade: %s\n", letterGrade);
}

// mult student grade calc
void optionMultipleStudents(void)
{
    int m = 0;
    printf("\nEnter number of course components: ");
    if (scanf("%d", &m) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    char compNames[m][COMP_NAME_SIZE];
    double weight[m], maxPoints[m];
    componentInputOption2(m, compNames, weight, maxPoints);
    
    // clear previously stored records
    if (students != NULL) {
        free(students);
        students = NULL;
        numStudents = 0;
    }
    
    while (true)
    {
        char tempName[STUDENT_NAME_SIZE];
        printf("Enter student's full name (or type DONE to stop): ");
        fgets(tempName, STUDENT_NAME_SIZE, stdin);
        tempName[strcspn(tempName, "\n")] = '\0';
        
        if (strcmp(tempName, "DONE") == 0)
            break;
        
        double scores[m];
        // read scores for components without printing headr
        scoreInputOption2(m, compNames, maxPoints, scores);
        
        double finalPercentage = computeGradePercentage(m, weight, maxPoints, scores);
        int roundedGrade = (int) round(finalPercentage);
        char tempLetter[3] = " ";
        grade(roundedGrade, tempLetter);
        
        // store student record without printing summaries for each person
        Student *temp = realloc(students, (numStudents + 1) * sizeof(Student));
        if (temp == NULL)
        {
            fprintf(stderr, "Memory allocation error.\n");
            exit(1);
        }
        students = temp;
        strncpy(students[numStudents].name, tempName, STUDENT_NAME_SIZE);
        students[numStudents].finalGrade = roundedGrade;
        strcpy(students[numStudents].letterGrade, tempLetter);
        numStudents++;
    }
    
    if (numStudents > 0)
    {
        double *classGrades = malloc(numStudents * sizeof(double));
        if (classGrades == NULL) {
            fprintf(stderr, "Memory allocation error.\n");
            exit(1);
        }
        for (int i = 0; i < numStudents; i++)
            classGrades[i] = students[i].finalGrade;
        
        double max, avg, median;
        calculateStatistics(numStudents, classGrades, &max, &avg, &median);
        printf("\n-- Class Summary --\n");
        // print individual student records in input order
        for (int i = 0; i < numStudents; i++)
            printf("%s - %.0f%% (%s)\n", students[i].name, students[i].finalGrade, students[i].letterGrade);
        printf("Maximum: %.0f%% (Students: ", max);
        bool first = true;
        for (int i = 0; i < numStudents; i++)
        {
            if (fabs(students[i].finalGrade - max) < 0.001)
            {
                if (!first)
                    printf(", ");
                printf("%s", students[i].name);
                first = false;
            }
        }
        printf(")\n");
        printf("Average: %.0f%%\n", round(avg));
        printf("Median: %.0f%%\n", round(median));
        free(classGrades);
    }
    else
    {
        printf("No student records entered.\n");
    }
}

// sort students by grade n display rankings
void optionSortStudents(void)
{
    if (numStudents <= 0)
    {
        printf("\nNo student records available for sorting.\n");
        return;
    }
    
    qsort(students, numStudents, sizeof(Student), compareStudentsDesc);
    
    printf("======================================\n");
    printf("Sorting students by final grade...\n");
    printf("======================================\n");
    printf("Class Ranking:\n");
    for (int i = 0; i < numStudents; i++)
    {
        printf("%d. %s - %.0f%% (%s)\n", i + 1, students[i].name, students[i].finalGrade, students[i].letterGrade);
    }
}

// comparator for qsort in descendning order
int compareStudentsDesc(const void *a, const void *b)
{
    Student *s1 = (Student *) a;
    Student *s2 = (Student *) b;
    if (s1->finalGrade < s2->finalGrade)
        return 1;
    else if (s1->finalGrade > s2->finalGrade)
        return -1;
    else
        return 0;
}

// search for student by name
void optionSearchStudent(void)
{
    if (numStudents <= 0)
    {
        printf("\nNo student records available to search.\n");
        return;
    }
    
    char searchName[STUDENT_NAME_SIZE];
    printf("Enter student name to search: ");
    fgets(searchName, STUDENT_NAME_SIZE, stdin);
    searchName[strcspn(searchName, "\n")] = '\0';
    
    bool found = false;
    for (int i = 0; i < numStudents; i++)
    {
        if (strcmp(students[i].name, searchName) == 0)
        {
            printf("Result: %s - %.0f%% (%s)\n", students[i].name, students[i].finalGrade, students[i].letterGrade);
            found = true;
            break;
        }
    }
    if (!found)
        printf("Student not found.\n");
}

// input fns for option 1
void componentScoreInputOption1(int n, char compNames[][COMP_NAME_SIZE], double weight[], double maxPoints[], double scores[])
{
    double totalWeight;
    do {
        totalWeight = 0;
        for (int i = 0; i < n; i++)
        {
            printf("Component %d:\n", i + 1);
            
            // name
            printf(" Name: ");
            fgets(compNames[i], COMP_NAME_SIZE, stdin);
            compNames[i][strcspn(compNames[i], "\n")] = '\0';
            
            // weight
            while (true)
            {
                printf(" Weight (as percentage): ");
                if (scanf("%lf", &weight[i]) != 1)
                {
                    clearInputBuffer();
                    continue;
                }
                if (weight[i] <= 0 || weight[i] > 100)
                {
                    printf("Error: Weight must be between 1 and 100.\n");
                    continue;
                }
                break;
            }
            
            // max allocated pts
            while (true)
            {
                printf(" Max Allocated Points: ");
                if (scanf("%lf", &maxPoints[i]) != 1)
                {
                    clearInputBuffer();
                    continue;
                }
                if (maxPoints[i] <= 0)
                {
                    printf("Error: Maximum allocated points must be positive.\n");
                    continue;
                }
                break;
            }
            clearInputBuffer();
            
            // earned pts
            while (true)
            {
                printf(" Earned Points: ");
                if (scanf("%lf", &scores[i]) != 1)
                {
                    clearInputBuffer();
                    continue;
                }
                if (scores[i] < 0)
                {
                    printf("Error: Earned points cannot be negative. Please re-enter.\n");
                    continue;
                }
                if (scores[i] > maxPoints[i])
                {
                    printf("Error: Earned points cannot exceed maximum allocated points (%.0f). Please re-enter.\n", maxPoints[i]);
                    continue;
                }
                break;
            }
            clearInputBuffer();
            totalWeight += weight[i];
        }
        if (totalWeight != 100)
            printf("Error: Total weight must equal 100%%! You've entered %.2lf. Please re-enter all components.\n", totalWeight);
    } while (totalWeight != 100);
}

// input fns for option 2
void componentInputOption2(int n, char compNames[][COMP_NAME_SIZE], double weight[], double maxPoints[])
{
    double totalWeight;
    do {
        totalWeight = 0;
        for (int i = 0; i < n; i++)
        {
            printf("Component %d:\n", i + 1);
            printf(" Name: ");
            fgets(compNames[i], COMP_NAME_SIZE, stdin);
            compNames[i][strcspn(compNames[i], "\n")] = '\0';
            
            while (true)
            {
                printf(" Weight (as percentage): ");
                if (scanf("%lf", &weight[i]) != 1)
                {
                    clearInputBuffer();
                    continue;
                }
                if (weight[i] <= 0 || weight[i] > 100)
                {
                    printf("Error: Weight must be between 1 and 100.\n");
                    continue;
                }
                break;
            }
            
            while (true)
            {
                printf(" Max Allocated Points: ");
                if (scanf("%lf", &maxPoints[i]) != 1)
                {
                    clearInputBuffer();
                    continue;
                }
                if (maxPoints[i] <= 0)
                {
                    printf("Error: Maximum allocated points must be positive.\n");
                    continue;
                }
                break;
            }
            clearInputBuffer();
            totalWeight += weight[i];
        }
        if (totalWeight != 100)
            printf("Error: Total weight must equal 100%%! You've entered %.2lf. Please re-enter all components.\n", totalWeight);
    } while (totalWeight != 100);
}

void scoreInputOption2(int n, char compNames[][COMP_NAME_SIZE], double maxPoints[], double scores[])
{
    for (int i = 0; i < n; i++)
    {
        while (true)
        {
            printf("Score for \"%s\": ", compNames[i]);
            if (scanf("%lf", &scores[i]) != 1)
            {
                clearInputBuffer();
                continue;
            }
            if (scores[i] < 0)
            {
                printf("Error: Score cannot be negative. Please re-enter.\n");
                continue;
            }
            if (scores[i] > maxPoints[i])
            {
                printf("Error: Score cannot exceed maximum allocated points (%.0f). Please re-enter.\n", maxPoints[i]);
                continue;
            }
            break;
        }
        clearInputBuffer();
    }
}

// grade calc n stats
double computeGradePercentage(int n, double weight[], double maxPoints[], double scores[])
{
    if (n <= 0)
        return 0;
    return ((scores[n - 1] / maxPoints[n - 1]) * weight[n - 1]) + computeGradePercentage(n - 1, weight, maxPoints, scores);
}

void grade(int finalGrade, char letterGrade[])
{
    if (finalGrade >= 90)
        strcpy(letterGrade, "A+");
    else if (finalGrade >= 85)
        strcpy(letterGrade, "A");
    else if (finalGrade >= 80)
        strcpy(letterGrade, "A-");
    else if (finalGrade >= 75)
        strcpy(letterGrade, "B+");
    else if (finalGrade >= 70)
        strcpy(letterGrade, "B");
    else if (finalGrade >= 65)
        strcpy(letterGrade, "C+");
    else if (finalGrade >= 60)
        strcpy(letterGrade, "C");
    else if (finalGrade >= 50)
        strcpy(letterGrade, "D");
    else
        strcpy(letterGrade, "F");
}

void calculateStatistics(int n, double grades[], double *max, double *avg, double *median)
{
    sortGrades(grades, n);
    *max = grades[n - 1];
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += grades[i];
    *avg = sum / n;
    if (n % 2 == 0)
        *median = (grades[n/2 - 1] + grades[n/2]) / 2;
    else
        *median = grades[n/2];
}

void sortGrades(double grades[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (grades[i] > grades[j])
            {
                double temp = grades[i];
                grades[i] = grades[j];
                grades[j] = temp;
            }
        }
    }
}

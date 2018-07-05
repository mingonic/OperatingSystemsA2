#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Validation array that checks each entry in the end to see if the puzzle is valid
//There are 27 threads being created in this program
int validation_array[27] = {0};

typedef struct {
    int row;
    int column;
} parameters;

//Sudoku array that reads sudoku from file. 
int sudoku[9][9] = {0};

//Checks each row to see if it is valid
void *row(void* p) {
    parameters *data = (parameters*) p;
    int row = data->row;
    int col = data->column;

    //Error check
    if (col != 0 || row >=9) {
        pthread_exit(NULL);
    }

    //Checks for one unique number between 1 and 9 in each row
    int check = 0, j, i;
    for(j = 1; j < 10; j++) {
        for(i =0; i < 9; i++) {
            int val = sudoku[row][i];
            if (val < 1 || val > 9) {
                pthread_exit(NULL);
            }
            if(val == j){
                check++;
            }
        }
        if(check != 1){
            pthread_exit(NULL);
        }
        check = 0;
    }

    //Marks the specified array element to show row is valid
    validation_array[9 + row] = 1;
    pthread_exit(NULL);
}

//Checks each column to see if it is valid
void *column(void* p) {
    parameters *data = (parameters*) p;
    int row = data->row;
    int col = data->column;

    //Error check
    if (row != 0 || col >= 9) {
        pthread_exit(NULL);
    }

    //Checks for one unique number between 1 and 9 in each column
    int check = 0, j, i;
    for(j = 1; j < 10; j++) {
        for(i =0; i < 9; i++) {
            int val = sudoku[i][col];
            if (val < 1 || val > 9) {
                pthread_exit(NULL);
            }
            if(val == j){
                check++;
            }
        }
        if(check != 1){
            pthread_exit(NULL);
        }
        check = 0;
    }

    //Marks the specified array element to show column is valid
    validation_array[18 + col] = 1;
    pthread_exit(NULL);
}

//Checks each 3x3 square of the puzzle (total of 9) to see if they are valid
void *cube(void* p) {
    parameters *data = (parameters*) p;
    int row = data->row;
    int col = data->column;

    //Error check
    if (col % 3 != 0 || row % 3 != 0 || col > 6 || row > 6) {
        pthread_exit(NULL);
    }

    //Making the 3x3 matrix into an array of integers
    int cube[9] = {0};
    int i = 0, j, k;
    for (j = row; j < row + 3; j++) {
        for (k = col; k < col + 3; k++) {
            cube[i] = sudoku[j][k];
            i++;
        }
    }

    //Checks for one unique number between 1 and 9 in the 3x3 matrix
    int check = 0;
    for(j = 1; j < 10; j++) {
        for(i = 0; i < 9; i++) {
            int val = cube[i];
            if (val < 1 || val > 9) {
                pthread_exit(NULL);
            }
            if(val == j){
                check++;
            }
        }
        if(check != 1){
            pthread_exit(NULL);
        }
        check = 0;
    }

    //Marks the specified array element to show that the 3x3 matrix is valid
    validation_array[row + col/3] = 1;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    //Initializes 27 threads (9 for row, 9 for col and 9 for each 3x3 square matrix)
    pthread_t threads[27];
    int threadNumber = 0;
    int i, j;
    char file_name[30];

    printf ("Please enter name of file with the sudoku puzzle (ex: sud_sol.txt):\n");
    scanf ("%s", file_name);

    //Reading the sudoku from file
    FILE *f = fopen (file_name, "r");
    if (f == NULL) {
        fprintf(stderr, "Error opening file: %s\n", file_name);
        return 0;
    } else {
        i = 0;
        while (!feof (f) && i < 9) {
            for (j = 0; j < 9; j++) {
            fscanf(f, "%d", &sudoku[i][j]);
            }
            i++;
        }
    }
    fclose (f);

    //Creating the 27 threads
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            if (i%3 == 0 && j%3 == 0) {
                parameters *data = (parameters *) malloc(sizeof(parameters));
                data->row = i;
                data->column = j;

                //creating thread for each 3x3 matrix
                pthread_create(&threads[threadNumber++], NULL, cube, data);
            }
            if (j == 0) {
                parameters *rowData = (parameters *) malloc(sizeof(parameters));
                rowData->row = i;
                rowData->column = j;
                
                //creating thread for each row
                pthread_create(&threads[threadNumber++], NULL, row, rowData);
            }
            if (i == 0) {
                parameters *columnData = (parameters *) malloc(sizeof(parameters));
                columnData->row = i;
                columnData->column = j;

                //creating thread for each column
                pthread_create(&threads[threadNumber++], NULL, column, columnData);
            }
        }
    }

    //Waiting for the threads to finish
    for (i = 0; i < 27; i++) {
        pthread_join(threads[i], NULL);
    }

    // If a number in the validation array is 0 that means the board is invalid, otherwise it is valid
    for (i = 0; i < 27; i++) {
        if (validation_array[i] == 0) {
        printf("Sudoku puzzle from file %s is NOT valid!\n", file_name);
        return 0;
        }
    }
    printf("Sudoku puzzle from file %s is valid.\n", file_name);
    return 0;
}
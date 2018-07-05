/**
 *Design a multithreaded Sudoku Solution Checker program
 *that accepts a Sudoku solution from the user as input,
 *determines whether the solution us valid or invalid.
 */

/**
 *Approach:
 *Using 27 threads this program determines is a Sudoku Solution
 *is correct. This program uses 9 threads for each of the 9 columns
 *and 9 for each of the 9 rows. Each block contains a 3x3 set thus 27
 *threads will be needed in total.
 *Using a global array to see if the threads return valide by return 1
 *if row is valide. If not the solution is not valide.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define num_threads 27

//This array is used for the validation step
int valid[num_threads] = {0};

// Struct that stores the data to be passed to threads
typedef struct {

  int row;
  int column;

} parameters;

// test example
int sudoku[9][9] = {
  {1, 5, 2, 4, 8, 9, 3, 7, 6},
  {7, 3, 9, 2, 5, 6, 8, 4, 1},
  {4, 6, 8, 3, 7, 1, 2, 9, 5},
  {3, 8, 7, 1, 2, 4, 6, 5, 9},
  {5, 9, 1, 7, 6, 3, 4, 2, 8},
  {2, 4, 6, 8, 9, 5, 7, 1, 3},
  {9, 1, 4, 6, 3, 7, 5, 8, 2},
  {6, 2, 5, 9, 4, 8, 1, 3, 7},
  {8, 7, 3, 5, 1, 2, 9, 6, 4}
};

// checks is only 1 appearence of 1-9 in column
void *isColumnValid(void* param) {

  // Confirm that parameters indicate a valid col subsection
  parameters *params = (parameters*) param;
  int row = params->row;
  int col = params->column;
  if (row != 0 || col > 8) {
    fprintf(stderr, "Invalid row or column for col subsection! row=%d, col=%d\n", row, col);
    pthread_exit(NULL);
  }

  // Check if numbers 1-9 only appear once in the column
  int validityArray[9] = {0};
  int i;
  for (i = 0; i < 9; i++) {
    int num = sudoku[i][col];
    if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
      pthread_exit(NULL);
    } else {
      validityArray[num - 1] = 1;
    }
  }

  // If reached this point, col subsection is valid.
  valid[18 + col] = 1;
  pthread_exit(NULL);
}

// checks is only 1 appearence of 1-9 in row
void *isRowValid(void* param) {

  // Confirm that parameters indicate a valid row subsection
  parameters *params = (parameters*) param;
  int row = params->row;
  int col = params->column;
  if (col != 0 || row > 8) {
    fprintf(stderr, "Invalid row or column for row subsection! row=%d, col=%d\n", row, col);
    pthread_exit(NULL);
  }

  // Check if numbers 1-9 only appear once in the row
  int validityArray[9] = {0};
  int i;
  for (i = 0; i < 9; i++) {
    // If the corresponding index for the number is set to 1, and the number is encountered again,
    // the valid array will not be updated and the thread will exit.
    int num = sudoku[row][i];
    if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
      pthread_exit(NULL);
    } else {
      validityArray[num - 1] = 1;
    }
  }
  // If reached this point, row subsection is valid.
  valid[9 + row] = 1;
  pthread_exit(NULL);
}

// checks is only 1 appearence of 1-9 in 3x3 blocks
void *is3x3Valid(void* param) {

  // Confirm that parameters indicate a valid 3x3 subsection
  parameters *params = (parameters*) param;
  int row = params->row;
  int col = params->column;
  if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0) {
    fprintf(stderr, "Invalid row or column for subsection! row=%d, col=%d\n", row, col);
    pthread_exit(NULL);
  }
  int validityArray[9] = {0};
  int i, j;
  for (i = row; i < row + 3; i++) {
    for (j = col; j < col + 3; j++) {
      int num = sudoku[i][j];
      if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
        pthread_exit(NULL);
      } else {
        validityArray[num - 1] = 1;
      }
    }
  }
  // If reached this point, 3x3 subsection is valid.
  valid[row + col/3] = 1; // Maps the subsection to an index in the first 8 indices of the valid array
  pthread_exit(NULL);
}


int main(int argc, char* argv[]) {

  system("cls");
  pthread_t threads[num_threads];
  int threadIndex = 0;
  int i,j;
  printf("\nThe Sudoku Board is displayed below.....\n");
  for(i=0;i<9;i++){
    for(j=0;j<9;j++){
      printf("%d\t", sudoku[i][j]);
    }
    printf("\n");
  }
  // Creats the 27 threads
  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      if (i%3 == 0 && j%3 == 0) {
        parameters *data = (parameters *) malloc(sizeof(parameters));
        data->row = i;
        data->column = j;
        // 3x3 subsection threads
        pthread_create(&threads[threadIndex++], NULL, is3x3Valid, data);
      }
      if (i == 0) {
        parameters *columnData = (parameters *) malloc(sizeof(parameters));
        columnData->row = i;
        columnData->column = j;
        // column threads
        pthread_create(&threads[threadIndex++], NULL, isColumnValid, columnData);
      }
      if (j == 0) {
        parameters *rowData = (parameters *) malloc(sizeof(parameters));
        rowData->row = i;
        rowData->column = j;
        // row threads
        pthread_create(&threads[threadIndex++], NULL, isRowValid, rowData);
      }
    }
  }
  for (i = 0; i < num_threads; i++) {
    // Wait for finish
    pthread_join(threads[i], NULL);
  }
  // if any number is 0 then invalid
  for (i = 0; i < num_threads; i++) {
    if (valid[i] == 0) {
      printf("Sudoku Board is invalid!\n");
      return EXIT_SUCCESS;
    }
  }
  printf("Sudoku Board is valid!\n");
  system("PAUSE");
  return EXIT_SUCCESS;
}

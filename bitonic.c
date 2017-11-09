/**
* CSCI 251
* Fall 2017
* Project 2
*
* David Kisluk
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void swap(int *array, int indexA, int indexB) {
  int temp = array[indexA];
  array[indexA] = array[indexB];
  array[indexB] = temp;
  // printf("swap(%d, %d)\n", array[indexA], array[indexB]);
}

void desc_swap(int *array, int indexA, int indexB) {
  if (array[indexA] > array[indexB]) {
    swap(array, indexA, indexB);
  }
}

void asc_swap(int *array, int indexA, int indexB) {
  if (array[indexA] < array[indexB]) {
    swap(array, indexA, indexB);
  }
}

void print_seq(int *array, int length) {
  for (int i=0; i < length; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}

void to_bitonic_seq(int *array, int length, int thread_count) {
  int num_stages = (int)(log2(length) - 1);
  int rows_per_thread = length / thread_count;

  // Stages
  for (int s = 1; s <= num_stages; s++) {
    int step = (int)pow(2, s);
    int comparisons_per_step = step / 2;

    // Ranges?
    for (int r = s; r > 0; r--) {
      printf("##################\n");
      printf("Stage %d\tRound %d\n", s, r);
      printf("##################\n");
      int pair_offset = (int)pow(2, r-1);
      int row = 0;

      // Alternating
      int AD = 1;
      for (int a = 0; a < length; a += step) {
        int b = a + pair_offset;
        int coef = 1; //(r > 1) ? 1 : 2;
        int offset = 0;
        int d = 0; // kind of like the column index?

        for (int c = 0; c < comparisons_per_step; c++) {
          if ((d+1) > pair_offset) {
            offset += pair_offset;
            d = 0;
          }
          int indexA = a+c*coef+offset;
          int indexB = b+c*coef+offset;
          int pid = row / rows_per_thread;
          printf("[P%d]\t", pid);
          if (AD > 0) {
            printf("DESC\t%d\n\t\t%d\t%d\t%d\n", indexA, indexB, array[indexA], array[indexB]);
            desc_swap(array, indexA, indexB);
          } else {
            printf("ASC\t%d\n\t\t%d\t%d\t%d\n", indexA, indexB, array[indexA], array[indexB]);
            asc_swap(array, indexA, indexB);
          }
          d++;
          row++;
        }

        AD *= -1; // DESC <-> ASC
      }
    }
  }
}

void bitonic_sort(int* array, int length, int thread_count) {
  // ensure the array is a bitonic sequence before sorting
  to_bitonic_seq(array, length, thread_count);

  int s = (int)log2(length);
  int step = (int)pow(2, s);
  int comparisons_per_step = step / 2;

  // Ranges?
  for (int r = s; r > 0; r--) {
    printf("##################\n");
    printf("Stage %d\tRound %d\n", s, r);
    printf("##################\n");
    int pair_offset = (int)pow(2, r-1);
    int row = 0;

    for (int a = 0; a < length; a += step) {
      int b = a + pair_offset;
      int coef = 1; //(r > 1) ? 1 : 2;
      int offset = 0;
      int d = 0; // kind of like the column index?

      for (int c = 0; c < comparisons_per_step; c++) {
        if ((d+1) > pair_offset) {
          offset += pair_offset;
          d = 0;
        }
        int indexA = a+c*coef+offset;
        int indexB = b+c*coef+offset;
        printf("DESC\t%d\n\t\t%d\t%d\t%d\n", indexA, indexB, array[indexA], array[indexB]);
        desc_swap(array, indexA, indexB);
        d++;
        row++;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  /* Validate arguments */
  if (argc != 3) {
    printf("Error: 2 argument expected.\n");
    printf("Usage: ./bitonic <input_file> <thread_count>\n");
    exit(1);
  }

  /* Open file */
  FILE *pFile;
  pFile = fopen(argv[1], "r");

  /* Parse array dimension */
  int N;
  fscanf(pFile, "%d", &N);

  /* Parse array A */
  int *A = malloc(sizeof(int[N]));

  int e;
  for (int i=0; i<N; ++i) {
    fscanf(pFile, "%d", &e);
    A[i] = e;
  }

  int numberOfThreads = atoi(argv[2]);

  print_seq(A, N);
  bitonic_sort(A, N, numberOfThreads);
  print_seq(A, N);

  /* Close file */
  fclose(pFile);

  free(A);

  return 0;
}

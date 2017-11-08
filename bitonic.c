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
}

void desc_swap(int *array, int indexA, int indexB) {
  printf("descending? %d -> %d", array[indexA], array[indexB]);
  if (array[indexA] > array[indexB]) {
    printf(" NO! Swapping...");
    swap(array, indexA, indexB);
  }
  printf("\n");
}

void asc_swap(int *array, int indexA, int indexB) {
  printf("descending? %d -> %d", array[indexA], array[indexB]);
  if (array[indexA] < array[indexB]) {
    printf(" NO! Swapping...");
    swap(array, indexA, indexB);
  }
  printf("\n");
}

void print_seq(int *array, int length) {
  for (int i=0; i < length; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}

void to_bitonic_seq(int *array, int length) {
  int num_stages = (int)(log2(length) - 1);

  // Stages
  for (int s = 1; s <= num_stages; s++) {
    int step = (int)pow(2, s+1);
    int comparisons_per_step = step / 2;

    // Ranges?
    for (int r = s; r > 0; r--) {
      printf("Stage %d\tRound %d\n", s, r);
      int pair_offset = (int)pow(2, r-1);

      // Descending
      for (int a = 0; a < length; a += step) {
        int b = a + pair_offset;
        int inner_step = (r > 1) ? 1 : 2;
        for (int i = 0; i < comparisons_per_step/2; i++) {
          int indexA = a+i*inner_step;
          int indexB = b+i*inner_step;
          desc_swap(array, indexA, indexB);
          printf("DESC(%d, %d)\n", indexA, indexB);
        }
      }

      // Ascenging
      for (int a = step/2; a < length; a += step) {
        int b = a + pair_offset;
        int inner_step = (r > 1) ? 1 : 2;
        for (int i = 0; i < comparisons_per_step/2; i++) {
          int indexA = a+i*inner_step;
          int indexB = b+i*inner_step;
          asc_swap(array, indexA, indexB);
          printf("ASC(%d, %d)\n", indexA, indexB);
        }
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

  print_seq(A, N);
  to_bitonic_seq(A, N);
  print_seq(A, N);

  /* Close file */
  fclose(pFile);

  int numberOfThreads = atoi(argv[2]);

  free(A);

  return 0;
}

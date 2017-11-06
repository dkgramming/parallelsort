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
          printf("DESC(%d, %d)\n", a+i*inner_step, b+i*inner_step);
        }
      }

      // Ascenging
    }
  }
}

int main(int argc, char *argv[]) {
  /* Validate arguments */
  if (argc != 3) {
    printf("Error: 1 argument expected.\n");
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

  to_bitonic_seq(A, N);

  /* Close file */
  fclose(pFile);

  int numberOfThreads = atoi(argv[2]);

  free(A);

  return 0;
}

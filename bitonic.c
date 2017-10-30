/**
* CSCI 251
* Fall 2017
* Project 2
*
* David Kisluk
*/
#include <stdio.h>
#include <stdlib.h>

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
  double *A = malloc(sizeof(double[N]));

  int e;
  for (int i=0; i<N; ++i) {
    fscanf(pFile, "%d", &e);
    A[i] = e;
    printf("%d ", e);
  }

  /* Close file */
  fclose(pFile);

  int numberOfThreads = atoi(argv[2]);

  free(A);

  return 0;
}

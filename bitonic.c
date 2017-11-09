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
#include <pthread.h>
#include <assert.h>

typedef struct {
  int id;
  int* array;
  int length;
  int step;
  int pair_offset;
  int comparisons_per_step;
  int task_count;
  int thread_count;
  int AD;
  int s;
  int num_stages;
} threadData_t;

void swap(int *array, int indexA, int indexB) {
  int temp = array[indexA];
  array[indexA] = array[indexB];
  array[indexB] = temp;
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

int thread_id(int task_id, int task_count, int thread_count) {
  int tasks_per_thread = task_count / thread_count;
  return task_id / tasks_per_thread;
}

void * bitonic_worker(void *arg) {
  threadData_t *td = (threadData_t *)arg;
  int id                    = td->id;
  int* array                = td->array;
  int length                = td->length;
  int step                  = td->step;
  int pair_offset           = td->pair_offset;
  int comparisons_per_step  = td->comparisons_per_step;
  int task_count            = td->task_count;
  int thread_count          = td->thread_count;
  int AD                    = td->AD;
  int s                     = td->s;
  int num_stages            = td->num_stages;

  for (int a = 0; a < length; a += step) {
    int b = a + pair_offset;
    int offset = 0;
    int d = 0; // kind of like the column index?

    for (int c = 0; c < comparisons_per_step; c++) {
      int row = a / 2 + c;
      if (d == pair_offset) {
        offset += pair_offset;
        d = 0;
      }
      int indexA = a + c + offset;
      int indexB = b + c + offset;

      if (id == thread_id(row, task_count, thread_count)) {
        if (AD > 0 || s == num_stages) {
          desc_swap(array, indexA, indexB);
        } else {
          asc_swap(array, indexA, indexB);
        }
      }
      d++;
    }

    AD *= -1; // DESC <-> ASC
  }
}

void bitonic_sort(int *array, int length, int thread_count) {
  int task_count = length / 2;
  int num_stages = (int)log2(length);

  // Prepare the thread pool
  pthread_t *thread = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
  threadData_t *arg = (threadData_t *)malloc(thread_count * sizeof(threadData_t));

  // Stages
  for (int s = 1; s <= num_stages; s++) {
    int step = (int)pow(2, s);
    int comparisons_per_step = step / 2;

    // Ranges?
    for (int r = s; r > 0; r--) {
      int pair_offset = (int)pow(2, r-1);

      // Alternating
      int AD = 1;

      for (int x = 0; x < thread_count; ++x) {
        arg[x].id                   = x;
        arg[x].array                = array;
        arg[x].length               = length;
        arg[x].step                 = step;
        arg[x].pair_offset          = pair_offset;
        arg[x].comparisons_per_step = comparisons_per_step;
        arg[x].task_count           = task_count;
        arg[x].thread_count         = thread_count;
        arg[x].AD                   = AD;
        arg[x].s                    = s;
        arg[x].num_stages           = num_stages;

        pthread_create(&thread[x], NULL, bitonic_worker, (void *)(arg+x));
      }
      for (int x = 0; x < thread_count; ++x) {
        pthread_join(thread[x], NULL);
      }
    }
  }
}

int max_power_of_2(int n) {
  int res = 1;

  // Try all powers starting from 2^1
  for (int i=0; i==i; i++) {
    int curr = 1 << i;

    // If current power is more than n, break
    if (curr > n) {
      break;
    }

    res = curr;
  }

  return res;
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

  int requested_thread_count = atoi(argv[2]);
  int actual_thread_count = max_power_of_2(requested_thread_count);
  actual_thread_count = (actual_thread_count > N/2) ? N/2 : actual_thread_count;

  bitonic_sort(A, N, actual_thread_count);

  if (requested_thread_count != actual_thread_count) {
    printf("Note: %d threads were requested, but only %d were used.\n",
         requested_thread_count, actual_thread_count);
  }

  /* Close file */
  fclose(pFile);

  free(A);

  return 0;
}

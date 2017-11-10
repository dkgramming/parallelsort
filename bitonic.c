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

const int DESC = 1;
const int ASC = -1;

typedef struct {
  int id;
  int* array;
  int length;
  int step;
  int pair_offset;
  int comparisons_per_step;
  int task_count;
  int thread_count;
  int direction;
  int stage;
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
  int direction             = td->direction;
  int stage                 = td->stage;
  int num_stages            = td->num_stages;

  for (int a = 0; a < length; a += step) {
    int b = a + pair_offset;
    int offset = 0;
    int column = 0;

    for (int c = 0; c < comparisons_per_step; c++) {
      int row = a / 2 + c;
      if (column == pair_offset) {
        offset += pair_offset;
        column = 0;
      }
      int indexA = a + c + offset;
      int indexB = b + c + offset;

      if (id == thread_id(row, task_count, thread_count)) {
        if (direction == DESC || stage == num_stages) {
          desc_swap(array, indexA, indexB);
        } else if (direction == ASC) {
          asc_swap(array, indexA, indexB);
        }
      }
      column++;
    }

    direction *= -1; // DESC <-> ASC
  }
}

void bitonic_sort(int *array, int length, int thread_count) {
  int task_count = length / 2;
  int num_stages = (int)log2(length);

  // Prepare the thread pool
  pthread_t *thread = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
  threadData_t *arg = (threadData_t *)malloc(thread_count * sizeof(threadData_t));

  // Stages
  for (int stage = 1; stage <= num_stages; stage++) {
    int step = (int)pow(2, stage);
    int comparisons_per_step = step / 2;

    // Ranges?
    for (int range = stage; range > 0; range--) {
      int pair_offset = (int)pow(2, range-1);

      // Alternating
      int direction = DESC;

      for (int id = 0; id < thread_count; ++id) {
        arg[id].id                   = id;
        arg[id].array                = array;
        arg[id].length               = length;
        arg[id].step                 = step;
        arg[id].pair_offset          = pair_offset;
        arg[id].comparisons_per_step = comparisons_per_step;
        arg[id].task_count           = task_count;
        arg[id].thread_count         = thread_count;
        arg[id].direction            = direction;
        arg[id].stage                = stage;
        arg[id].num_stages           = num_stages;

        pthread_create(&thread[id], NULL, bitonic_worker, (void *)(arg+id));
      }
      for (int id = 0; id < thread_count; ++id) {
        pthread_join(thread[id], NULL);
      }
    }
  }

  free(thread);
  free(arg);
}

int max_power_of_2(int max) {
  int result = 1;

  // Try all powers of 2
  for (int exponent = 0;; exponent++) {
    // 2^exponent
    int current = 1 << exponent;

    if (current > max) { break; }

    result = current;
  }

  return result;
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
  int input_size;
  fscanf(pFile, "%d", &input_size);

  /* Parse array A */
  int *array = malloc(sizeof(int[input_size]));

  int element;
  for (int i=0; i<input_size; ++i) {
    fscanf(pFile, "%d", &element);
    array[i] = element;
  }

  int requested_thread_count = atoi(argv[2]);
  int actual_thread_count = max_power_of_2(requested_thread_count);
  actual_thread_count = (actual_thread_count > input_size/2) ?
                          input_size/2 :
                          actual_thread_count;

  bitonic_sort(array, input_size, actual_thread_count);
  print_seq(array, input_size);

  if (requested_thread_count != actual_thread_count) {
    printf("Note: %d threads were requested, but only %d were used.\n",
         requested_thread_count, actual_thread_count);
  }

  /* Close file */
  fclose(pFile);

  free(array);

  return 0;
}

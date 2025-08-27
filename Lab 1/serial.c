#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>

#define NUM_RUNS 10

// Node structure
struct list_node_s {
    int data;
    struct list_node_s* next;
};

int Member(int value, struct list_node_s* head_p){
    struct list_node_s* curr_p = head_p;
    while (curr_p != NULL && curr_p->data < value){
        curr_p = curr_p->next;
    }
    if (curr_p != NULL && curr_p->data == value){
        return 1;
    } else {
        return 0;
    }
}

int Insert(int value, struct list_node_s** head_pp){
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    // Traverse to the correct position
    while (curr_p != NULL && curr_p->data < value){
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    // If value already exists, do not insert
    if (curr_p != NULL && curr_p->data == value) {
        return 0;
    }
    // Allocate new node
    temp_p = malloc(sizeof(struct list_node_s));
    temp_p->data = value;
    temp_p->next = curr_p;
    if (pred_p == NULL){
        *head_pp = temp_p;
    } else {
        pred_p->next = temp_p;
    }
    return 1;
}

int Delete(int value, struct list_node_s** head_pp){
  struct list_node_s* curr_p = *head_pp;
  struct list_node_s* pred_p = NULL;
  while (curr_p != NULL && curr_p->data < value){
    pred_p = curr_p;
    curr_p = curr_p->next;
  }
  if (curr_p != NULL && curr_p->data == value){
    if (pred_p ==NULL){
      *head_pp = curr_p->next;
      free(curr_p);
    } else {
      pred_p->next = curr_p->next;
      free(curr_p);
    }
    return 1;
  } else {
    return 0;
  }
}

// Helper function to generate a random number in [0, 65535]
int get_random_value() {
  return rand() % 65535;
}

// Helper function to check if value is in the list
int is_in_list(int value, struct list_node_s* head) {
  struct list_node_s* curr = head;
  while (curr != NULL) {
    if (curr->data == value) return 1;
    curr = curr->next;
  }
  return 0;
}

// Helper function to print elements in the linked list
// void print_list(struct list_node_s* head) {
//     struct list_node_s* curr = head;
//     printf("List: ");
//     while (curr != NULL) {
//         printf("%d ", curr->data);
//         curr = curr->next;
//     }
//     printf("\n");
// }

int main(int argc, char* argv[]) {
  printf("Starting the program...\n");
  int n = 10; // Number of initial unique values
  int m = 10; // Number of operations
  float mMember = 0.8, mInsert = 0.1, mDelete = 0.1; // Fractions
  struct list_node_s* head = NULL;
  int inserted = 0;
  int value;
  int i;
  int op;
  int member_count = 0, insert_count = 0, delete_count = 0;

  if (argc >= 5) {
    n = atoi(argv[1]);
    m = atoi(argv[2]);
    mMember = atof(argv[3]);
    mInsert = atof(argv[4]);
    mDelete = 1.0f - mMember - mInsert;
  }

  // Set a different random seed for each execution
  srand((unsigned int)time(NULL));

  double times[NUM_RUNS];
  double sum = 0.0, mean = 0.0, stddev = 0.0;

  for (int run = 0; run < NUM_RUNS; run++) {
    // Re-initialize the list for each run
    struct list_node_s* head = NULL;
    int inserted = 0;
    int value;
    int member_count = 0, insert_count = 0, delete_count = 0;

    // Populate the list with n unique random values
    while (inserted < n) {
      value = get_random_value();
      if (!is_in_list(value, head)) {
        Insert(value, &head);
        inserted++;
      }
    }

    // Timing variables
    LARGE_INTEGER freq, start, end;
    double elapsed_microseconds;

    QueryPerformanceFrequency(&freq);

    // Perform m random operations and measure time
    QueryPerformanceCounter(&start);
    for (i = 0; i < m; i++) {
      float prob = (float)rand() / RAND_MAX;
      value = get_random_value();
      if (prob < mMember) {
        Member(value, head);
        member_count++;
      } else if (prob < mMember + mInsert) {
        if (!is_in_list(value, head)) {
          Insert(value, &head);
          insert_count++;
        }
      } else {
        Delete(value, &head);
        delete_count++;
      }
    }
    QueryPerformanceCounter(&end);

    elapsed_microseconds = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / freq.QuadPart;
    times[run] = elapsed_microseconds;
    sum += elapsed_microseconds;

    // Free the list
    struct list_node_s* curr = head;
    while (curr != NULL) {
      struct list_node_s* tmp = curr;
      curr = curr->next;
      free(tmp);
    }
  }

  // Calculate mean
  mean = sum / NUM_RUNS;

  // Calculate standard deviation
  for (int run = 0; run < NUM_RUNS; run++) {
    stddev += (times[run] - mean) * (times[run] - mean);
  }
  stddev = sqrt(stddev / NUM_RUNS);

  printf("Average execution time for %d runs: %.0f microseconds\n", NUM_RUNS, mean);
  printf("Standard deviation: %.0f microseconds\n", stddev);

  return 0;
}
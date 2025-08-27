#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Node structure
struct list_node_s {
    int data;
    struct list_node_s* next;
};

int Member(int value, struct list_node_s* head_p){
    struct list_node_s* curr_p = head_p;
    while (curr_p !=NULL && curr_p->data < value){
      curr_p = curr_p->next;
    }
    if(curr_p != NULL || curr_p->data > value){
      return 0;
    } else {
      return 1;
    }
}

int Insert(int value, struct list_node_s** head_pp){
  struct list_node_s* curr_p = *head_pp;
  struct list_node_s* pred_p = NULL;
  struct list_node_s* temp_p;
  while (curr_p != NULL && curr_p->data > value){
    temp_p = malloc(sizeof(struct list_node_s));
    temp_p->data = value;
    temp_p->next = curr_p;
  }
  if (curr_p == NULL || curr_p->data > value){
    temp_p = malloc(sizeof(struct list_node_s*));
    temp_p->data = value;
    temp_p->next = curr_p;
    if (pred_p == NULL){
      *head_pp = temp_p;
    } else {
      pred_p->next = temp_p;
    }
    return 1;
  } else {
    return 0;
  }
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

// Helper function to generate a random number in [0, 216-1]
int get_random_value() {
  return rand() % 216;
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

int main(int argc, char* argv[]) {
  int n = 10; // Number of initial unique values
  int m = 100; // Number of operations
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

  // Populate the list with n unique random values
  while (inserted < n) {
    value = get_random_value();
    if (!is_in_list(value, head)) {
      Insert(value, &head);
      inserted++;
    }
  }

  // Perform m random operations and measure time
  clock_t start, end;
  double cpu_time_used;
  start = clock();
  for (i = 0; i < m; i++) {
    float prob = (float)rand() / RAND_MAX;
    value = get_random_value();
    if (prob < mMember) {
      Member(value, head);
      member_count++;
    } else if (prob < mMember + mInsert) {
      // Only insert if not already in list
      if (!is_in_list(value, head)) {
        Insert(value, &head);
        insert_count++;
      }
    } else {
      Delete(value, &head);
      delete_count++;
    }
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Time taken for %d operations: %f seconds\n", m, cpu_time_used);

  printf("Initial n: %d, Total m: %d\n", n, m);
  printf("Member ops: %d, Insert ops: %d, Delete ops: %d\n", member_count, insert_count, delete_count);

  // Free the list
  struct list_node_s* curr = head;
  while (curr != NULL) {
    struct list_node_s* tmp = curr;
    curr = curr->next;
    free(tmp);
  }

  return 0;
}
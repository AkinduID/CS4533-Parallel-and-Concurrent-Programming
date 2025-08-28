#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define NUM_RUNS 10

struct list_node_s {
    int data;
    struct list_node_s* next;
};

pthread_rwlock_t list_rwlock;

int Member(int value, struct list_node_s* head_p) {
    struct list_node_s* curr_p = head_p;
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;
    return (curr_p != NULL && curr_p->data == value) ? 1 : 0;
}

int Insert(int value, struct list_node_s** head_pp) {
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    if (curr_p != NULL && curr_p->data == value) return 0;
    temp_p = malloc(sizeof(struct list_node_s));
    temp_p->data = value;
    temp_p->next = curr_p;
    if (pred_p == NULL) *head_pp = temp_p;
    else pred_p->next = temp_p;
    return 1;
}

int Delete(int value, struct list_node_s** head_pp) {
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) *head_pp = curr_p->next;
        else pred_p->next = curr_p->next;
        free(curr_p);
        return 1;
    }
    return 0;
}

int get_random_value() {
    return rand() % 65535;
}

int is_in_list(int value, struct list_node_s* head) {
    struct list_node_s* curr = head;
    while (curr != NULL) {
        if (curr->data == value) return 1;
        curr = curr->next;
    }
    return 0;
}

typedef enum { OP_MEMBER, OP_INSERT, OP_DELETE } op_type;
typedef struct { op_type type; int value; } operation_t;

struct list_node_s* head = NULL;
operation_t* ops;
int m, n, num_threads;

void* thread_work(void* rank) {
    long my_rank = (long) rank;
    int ops_per_thread = m / num_threads;
    int start = my_rank * ops_per_thread;
    int end = (my_rank == num_threads-1) ? m : start + ops_per_thread;

    for (int i = start; i < end; i++) {
        if (ops[i].type == OP_MEMBER) {
            pthread_rwlock_rdlock(&list_rwlock);
            Member(ops[i].value, head);
            pthread_rwlock_unlock(&list_rwlock);
        } else {
            pthread_rwlock_wrlock(&list_rwlock);
            if (ops[i].type == OP_INSERT && !is_in_list(ops[i].value, head))
                Insert(ops[i].value, &head);
            else if (ops[i].type == OP_DELETE)
                Delete(ops[i].value, &head);
            pthread_rwlock_unlock(&list_rwlock);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    n = 1000; m = 10000;
    float mMember = 0.99, mInsert = 0.005, mDelete = 0.005;
    num_threads = 4;

    if (argc >= 6) {
        n = atoi(argv[1]);
        m = atoi(argv[2]);
        mMember = atof(argv[3]);
        mInsert = atof(argv[4]);
        num_threads = atoi(argv[5]);
        mDelete = 1.0f - mMember - mInsert;
    }

    srand((unsigned int)time(NULL));
    pthread_rwlock_init(&list_rwlock, NULL);

    double times[NUM_RUNS], sum = 0.0, mean = 0.0, stddev = 0.0;

    for (int run = 0; run < NUM_RUNS; run++) {
        head = NULL;
        int inserted = 0;

        while (inserted < n) {
            int value = get_random_value();
            if (!is_in_list(value, head)) { Insert(value, &head); inserted++; }
        }

        ops = malloc(sizeof(operation_t) * m);
        for (int i = 0; i < m; i++) {
            float prob = (float)rand()/RAND_MAX;
            int value = get_random_value();
            if (prob < mMember) ops[i].type = OP_MEMBER;
            else if (prob < mMember + mInsert) ops[i].type = OP_INSERT;
            else ops[i].type = OP_DELETE;
            ops[i].value = value;
        }

        struct timeval start, end;
        gettimeofday(&start, NULL);

        pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
        for (long t = 0; t < num_threads; t++) pthread_create(&threads[t], NULL, thread_work, (void*)t);
        for (long t = 0; t < num_threads; t++) pthread_join(threads[t], NULL);

        gettimeofday(&end, NULL);
        double elapsed = (end.tv_sec - start.tv_sec)*1e6 + (end.tv_usec - start.tv_usec);
        times[run] = elapsed; sum += elapsed;

        free(ops);
        struct list_node_s* curr = head;
        while (curr != NULL) { struct list_node_s* tmp = curr; curr = curr->next; free(tmp); }
    }

    mean = sum / NUM_RUNS;
    for (int run = 0; run < NUM_RUNS; run++) stddev += (times[run]-mean)*(times[run]-mean);
    stddev = sqrt(stddev / NUM_RUNS);

    printf("RWLock Implementation: Threads=%d, Runs=%d\n", num_threads, NUM_RUNS);
    printf("Average: %.0f microseconds\n", mean);
    printf("Std dev: %.0f microseconds\n", stddev);

    pthread_rwlock_destroy(&list_rwlock);
    return 0;
}

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct proc {
    pthread_t thread_id;
    int priority;
    int flag;
} proc_t;


void dummyFunc2();
void dummyFunc3();

void dummyFunc() {
    // printf("dummyFunc %p\n", dummyFunc);
    dummyFunc2();
}

void dummyFunc2() {
    dummyFunc3();
}
void dummyFunc3() {
    int x = 0;
    while (1) {
        x++;
    }
}
#define N 10
// global
proc_t proc[N];

FILE *file;

void *readThread(proc_t *thr) {


    while(1) {
        dummyFunc();
        thr->flag = rand()%2;
        sleep(1);

        if (thr->flag) {
            printf("t_id %d want to work\n", thr->priority);    
        }

        for (int i = 0; i < N; i++) {
            if (proc[i].flag == 1 && thr->flag == 1 && proc[i].priority > thr->priority && i != thr->priority) {
                printf("proc[%d] have work, so %d waiting..\n", i, thr->priority);
                while(proc[i].flag == 1) {
                    sleep(1);
                } 
            } 
        }
        char ch;
        fscanf(file, "%c", &ch);
        printf("%d done work: %c\n", thr->priority, ch);
        
    }
    
    
    printf("read thread priority = %d\n", thr->priority);

    return NULL;
}

void *writeThread(proc_t *thr) {

    long int write_pos = 0;
    long int read_pos;

    while(1) {
        thr->flag = rand()%4;
        sleep(1);

        if(thr->flag == 3) {
            printf("write thread got a job! (prio = %d)\n", thr->priority);

            // remember read pointer
            read_pos = ftell(file);

            fseek(file, write_pos, SEEK_SET);

            fprintf(file, "%d", 1);

            write_pos = ftell(file);

            // get read pointer back
            fseek(file, read_pos, SEEK_SET);

            printf("write job done\n");

        }
    }
    

    return NULL;
}

void main() {

    srand(time(NULL));

    file = fopen("text.txt", "r+");
    if(file == NULL) {
        printf("file == NULL\n");
        exit(5);
    }
    
    proc[0].priority = 99;

    pthread_create(&proc[0].thread_id, NULL, writeThread, &proc[0]);
    for (int i = 1; i < N; i++) {
        proc[i].priority = i;
        pthread_create(&proc[i].thread_id, NULL, readThread, &proc[i]);
    }
    printf("123\n");

    for (int i = 0; i < N; i++) {
        pthread_join(proc[i].thread_id, NULL);

    }

    fclose(file);
    exit(0);
    
}
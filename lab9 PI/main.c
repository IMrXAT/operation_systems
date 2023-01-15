#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define ITERATIONS_BETWEEN_CHECK

int flag = 0;

typedef struct num_iterations{
    int offset;
    int iterations;
    double part_sum;
}threads_work;

void* count_pi(void* args){
    threads_work *iterations = (threads_work *) args;
    printf("offset is %d\n", iterations->offset);
    for (int i = iterations->offset; i < iterations->iterations + iterations->offset; i++) {
        //printf("%d \n", i);
        iterations->part_sum += 1.0 / (i * 4.0 + 1.0);
        iterations->part_sum -= 1.0 / (i * 4.0 + 3.0);

    }

    pthread_exit((void*)0);
}

void init(threads_work* part_work, int num_iteration, int num_threads){
    for (int i = 0; i < num_threads; i++){
        if (i > 0) {
            part_work[i].offset = part_work[i - 1].offset + part_work[i - 1].iterations;
        }
        else{
            part_work[i].offset = 0;
        }
        part_work[i].iterations = (int)(num_iteration / num_threads);
        if (i < num_iteration % num_threads){
            part_work[i].iterations++;
        }
    }
}

int main(int argc, char **argv) {
    int num_iteration;
    int num_threads;

    if (argc > 0){
        num_threads = atoi(argv[1]);
        num_iteration = atoi(argv[2]);
        if (num_threads == 0){
            num_threads = 1;
        }
        if (num_iteration == 0){
            printf("enter number pls");
            scanf("%d", &num_iteration);
            return -1;
        }
    }
    else{
        return -1;
    }
    //printf("%d %d", num_threads, num_iteration);

    pthread_t *pthreads = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
    threads_work *part_work = (threads_work *) malloc(sizeof(threads_work) * num_threads);

    init(part_work, num_iteration, num_threads);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pthreads[i], NULL, count_pi, (void *) (&part_work[i]));
    }

    void *thread_return;
    double PI = 0;

    for (int i = 0; i < num_threads; i++) {
        pthread_join(pthreads[i], &thread_return);
        printf("part sum of thread %d == %.16f\n", i, part_work[i].part_sum);
        PI += part_work[i].part_sum;
    }
    printf("now i remember that pi is %.15f\n", PI * 4);

    free(pthreads);
    free(part_work);
}
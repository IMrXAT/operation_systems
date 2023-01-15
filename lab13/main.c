#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define STRINGS_TO_PRINT 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready = PTHREAD_COND_INITIALIZER;
bool is_parent_printed = false;

void* thr_print_text(void* args){
    pthread_mutex_lock(&mutex);
    for (int i = 1; i <= STRINGS_TO_PRINT; i++){
        while (!is_parent_printed){
            pthread_cond_wait(&ready, &mutex);
        }
        printf("second thread %d\n", i);
        is_parent_printed = false;
        pthread_cond_signal(&ready);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t new_tid;

    pthread_create(&new_tid, NULL, thr_print_text, NULL);

    sleep(1);

    pthread_mutex_lock(&mutex);
    for (int i = 1; i <= STRINGS_TO_PRINT; i++){
        printf("first thread %d\n", i);
        is_parent_printed = true;
        pthread_cond_signal(&ready);
        if (i != STRINGS_TO_PRINT){
            while (is_parent_printed){
                pthread_cond_wait(&ready, &mutex);
            }
        }
    }

    pthread_mutex_unlock(&mutex);

    pthread_join(new_tid, NULL);

    pthread_cond_destroy(&ready);
    pthread_mutex_destroy(&mutex);
    return 0;
}

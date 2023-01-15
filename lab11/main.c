#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define STRINGS_TO_PRINT 10
pthread_mutex_t mutex[3];
pthread_mutexattr_t mutex_attr[3];

void* thr_print_text(void* arg){

    pthread_mutex_lock(&mutex[2]);
    pthread_mutex_lock(&mutex[0]);
    for (int i = 1; i <= 10; i++){
        printf("second thread %d\n", i);
        pthread_mutex_unlock(&mutex[(i+1) % 3]);
        if (i != STRINGS_TO_PRINT){
            pthread_mutex_lock(&mutex[i % 3]);
        }

    }
    pthread_exit((void*)0);
}

int main() {
    pthread_t new_tid;
    for (int i = 0; i < 3; i++){
        pthread_mutex_init((mutex + i), NULL);
        pthread_mutexattr_init((mutex_attr + i));
        pthread_mutexattr_settype((mutex_attr + i), PTHREAD_MUTEX_ERRORCHECK);
    }

    pthread_mutex_lock(&mutex[0]);
    pthread_mutex_lock(&mutex[1]);
    int err = pthread_create(&new_tid, NULL, thr_print_text, NULL);
    if (err != 0){
        perror("can`t create new thread");
    }
    sleep(1);

    for (int i = 1; i <= 10; i++){
        printf("first thread %d\n", i);
        pthread_mutex_unlock(&mutex[(i-1) % 3]);
        pthread_mutex_lock(&mutex[(i+1) % 3]);
    }
    pthread_join(new_tid, NULL);
    for (int i = 0; i < 3; i++){
        pthread_mutexattr_destroy((mutex_attr+i));
        pthread_mutex_destroy((mutex+i));
    }
    return 0;
}
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* thr_print_text(void* arg){
    while (1){
        printf("stringgg\n");
    }
    //pthread_exit((void*)42);
}

int main() {
    pthread_t new_tid;
    void *tread_ret;
    int err = pthread_create(&new_tid, NULL, thr_print_text, NULL);
    if (err != 0){
        perror("can`t create new thread");
    }
    sleep(2);
    pthread_cancel(new_tid);
    err = pthread_join(new_tid, &tread_ret);
    if (err != 0){
        perror("cant join thread");
    }

    return 0;
}
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void cleanup(void* arg){
    printf("thread exit");
}

void *thr_print_text(void *arg) {
    pthread_cleanup_push(cleanup, NULL)
    while (1) {
                printf("stringgg\n");
            }
    pthread_cleanup_pop(0);
    //pthread_exit(NULL);
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
    if (err != 0 ){
        perror("cant join thread");
    }

    return 0;
}

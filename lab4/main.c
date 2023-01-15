#include <stdio.h>
#include <pthread.h>

#define BUFF_SIZE 1024
#define NUM_STRINGS 3
#define NUM_THREADS 4
typedef struct text{
    char string[NUM_STRINGS][BUFF_SIZE];
}Text;

void *thread_func(void *arg){
        Text *args = (Text*)arg;
        for (int i = 0; i < NUM_STRINGS; i++){
            printf("%s\n", args->string[i]);
        }
    pthread_exit((void*)0);
}

int main(int argc, char* argv[]) {
    pthread_t tidp[NUM_THREADS];
    int err;
    Text thread_text[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++){
        for (int j = 0; j < NUM_STRINGS; j++){
            scanf("%s", thread_text[i].string[j]);
            //printf("%s\n", thread_text[i].string[j]);
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        err = pthread_create(&tidp[i], NULL, thread_func, &thread_text[i]);
        if (err != 0){
            printf("can`t make thread\n");
            return err;
        }
    }
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(tidp[i], NULL);
    }

    return 0;
}

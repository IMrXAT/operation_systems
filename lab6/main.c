#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>



typedef struct param{
    int num_chars;
    char string[200];
}Param;



void* routine(void* args){
    Param* arg = (Param*) args;
    usleep(arg->num_chars * 3000);
    printf("%s\n", arg->string);
    pthread_exit(NULL);
}


int main() {
    pthread_t pthread[100];
    Param* param = (Param*) malloc(sizeof(Param)*100);
    int num_strings = 0;
    for (int i = 0; i < 100; i++){
        if (scanf("%s", param[i].string) == EOF){
            break;
        }
        param[i].num_chars = strlen(param[i].string);
        num_strings++;
    }
    for (int i = 0; i < num_strings; i++){
        pthread_create(&pthread[i], NULL, routine, (void*) &param[i]);
    }
    for(int i = 0; i < num_strings; i++){
        pthread_join(pthread[i], NULL);
    }
    free(param);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    long n = rand() % 100 ;

    for (int i = 0; i < n; i++){
        int k = rand() % 200;
        printf("%d.", i+1);

        for (int j = 0; j < k; j++){
            printf("%c", (char)(rand() % 26 + 'A'));
        }
        printf("\n");
    }
}

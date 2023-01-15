#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
typedef struct parsed_url_t{
    char* protocol;
    char* hostname;
    char* port_str;
}parsed_url_t;

#define MB 1000000
#define SCREEN_SIZE (80*25)
char buf[8*MB];
char text_block_to_print[SCREEN_SIZE];
int socket_desc;
int count_chars = 0;
int eof = 0;
int last_block_end = 0;


void generate_get_request(char *url);

void get_next_block();

// http://www.my_site.com:80/street/house/room/table
void parse_url(char* url, parsed_url_t *parsed_url) {
    char *token;
    char *host_and_port;
    char* copy_url = (char*) malloc(strlen(url));
    char *token_ptr;
    strcpy(copy_url, url);
    // get protocol
    token = strtok_r(copy_url, ":", &token_ptr);
    if (!token){
        perror("cant parse URL");
        exit(-1);
    }
    parsed_url->protocol = (char*) malloc (strlen(token)+1);
    strcpy(parsed_url->protocol, token);
    //printf("%s\n", parsed_url->protocol);

    // get host name and port number
    token = strtok_r(NULL, "/", &token_ptr);
    if (!token){
        perror("cant parse URL");
        exit(-1);
    }
    host_and_port = (char*) malloc(strlen(token)+1);
    strcpy(host_and_port, token);
    token = strtok(host_and_port, ":");
    if (token){
        parsed_url->hostname = token;
        parsed_url->port_str = strtok(NULL, "");
    }
    if (parsed_url->port_str == NULL){
        parsed_url->port_str = "80";
    }
//    //printf("%s\n%s\n", parsed_url->port_str, parsed_url->hostname);

}

int open_socket(char* hostname, char* port){
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    if (getaddrinfo(hostname, port, &hints, &result) != 0){
        perror("getaddrinfo failed");
        exit(-1);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next){
        socket_desc = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_desc == -1){
            continue;
        }
        if (connect(socket_desc, rp->ai_addr, rp->ai_addrlen) != -1){
            printf("connected\n");
            break;
        }
        close(socket_desc);
    }
    freeaddrinfo(result);
    return socket_desc;
}




//читать 

void* receive_data(void* args){
    char local_buf[80];
    int chars_read;
    while((chars_read = read(socket_desc, local_buf, 80)) > 0){
        memcpy((buf+count_chars+1), local_buf, chars_read);
        //printf("%s", local_buf);
        memset(local_buf, 0, 80);
        count_chars += chars_read;
    }
    eof = 1;
    pthread_exit(NULL);
}


void* print_data(void* args){
    char c;

    while(!eof){
        while (last_block_end < count_chars){
            do{
                printf("Press space to scroll down\n");
                read(STDIN_FILENO, &c, 1);
            } while (c != ' ');
            get_next_block();
            printf("%s\n", text_block_to_print);
        }
    }

    printf("\n");
    pthread_exit(NULL);
}


/*
 * считаем размер блока до 25 \n
 * копируем это количество символов от количества прочитанных до количество_прочитанных + размер блока
 *
 * */
void get_next_block() {
    int num_strings = 0;
    int block_size = 0;
    for (int i = 1; i < SCREEN_SIZE; i++){
        if (buf[last_block_end+i] == '\n'){
            num_strings++;

        }
        if (num_strings == 25 || last_block_end+i == count_chars){
            block_size = i;
            break;
        }
    }
    if (block_size == 0){
        block_size = SCREEN_SIZE;
    }
    memset(text_block_to_print, 0, SCREEN_SIZE);
    memcpy(text_block_to_print, (buf+last_block_end+1), block_size);
    last_block_end += block_size;
}


/*
HTTP запросы - это сообщения, отправляемые клиентом, чтобы инициировать реакцию со стороны сервера.
 Их стартовая строка состоит из трёх элементов:

1) Метод HTTP, глагол (например, GET, PUT или POST) или существительное (например, HEAD или OPTIONS),
описывающие требуемое действие. Например, GET указывает, что нужно доставить некоторый ресурс

2) Цель запроса, обычно URL, или абсолютный путь протокола, порт и домен обычно характеризуются контекстом запроса.
Формат цели запроса зависит от используемого HTTP-метода.
 Это может быть Абсолютный путь, за которым следует '?' и строка запроса.
 Это самая распространённая форма, называемая исходной формой (origin form)
 Используется с методами GET, POST, HEAD, и OPTIONS.
Полный URL - абсолютная форма (absolute form) , обычно используется с GET при подключении к прокси.

3) Версия HTTP, определяющая структуру оставшегося сообщения, указывая, какую версию предполагается использовать для ответа.
*/


void generate_get_request(char *url) {
    char *pattern = "GET %s HTTP/1.0\r\n\r\n";
    sprintf(buf, pattern, url);
    printf("%s\n", buf);
}

void http_client(char *hostname, char* port_str, char* url) {
    pthread_t receiver, writer;
    socket_desc = open_socket(hostname, port_str);

    generate_get_request(url);
    write(socket_desc, buf, strlen(buf));

    pthread_create(&receiver, NULL, receive_data, NULL);
    pthread_create(&writer, NULL, print_data, NULL);
    pthread_join(receiver, NULL);
    pthread_join(writer, NULL);

    close(socket_desc);
}



int main(int argc, char** argv) {

    struct termios term;
    struct termios save_term;
    if (isatty(STDIN_FILENO) == 0){
        perror("not terminal");
        exit(-1);
    }
    if (tcgetattr(STDIN_FILENO, &term) < 0){
        perror("tcgetattr error");
        exit(-1);
    }

    save_term = term;
    term.c_lflag &= ~(ICANON);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1){
        perror("tcsetattr error");
    }




    if (argc < 2){
        printf("did not enter URL\n");
    }
    char* url = argv[1];

    parsed_url_t *parsed_url = (parsed_url_t*) malloc(sizeof (parsed_url_t));
    parse_url(url, parsed_url);
    if (strcmp(parsed_url->protocol, "http") != 0){
        printf("only http\n");
        return 1;
    }

    http_client(parsed_url->hostname, parsed_url->port_str, url);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &save_term) == -1){
        perror("tcsetattr error");
    }
    return 0;
}
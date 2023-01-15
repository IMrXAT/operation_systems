#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

typedef struct PathInfo{
    char* src_path;
    char* dest_path;
}PathInfo;


PathInfo *get_next_paths(const char *source, const char *destination, const char *next_file_name);
char *build_path(const char *last_directory_path, char *delimiter, const char *new_file_name);
void *copy_files(void *args);
void destroyPaths(PathInfo *paths);
void copy_regular_file(PathInfo *paths, mode_t mode);
void copy_file_data(int source_fd, int dest_fd);
void copy_directory(PathInfo *paths, __mode_t mode);

int main(int argc, char** argv) {
    if (argc != 3){
        perror("wrong number of arguments");
        return -1;
    }

    PathInfo *paths = (PathInfo*) calloc(1, sizeof(PathInfo));
    if(paths == NULL){
        perror("get_next_paths error");
    }
    paths->src_path = argv[1];
    paths->dest_path = argv[2];
    if (strstr(paths->dest_path, paths->src_path) != NULL){
        perror("destination directory cant be in source directory");
        return -1;
    }
    printf("%s\n%s\n", paths->dest_path, paths->src_path);
    copy_files((void*)paths);
}

void *copy_files(void *args) {
    if (args == NULL){
        perror("invalid args");
        pthread_exit(NULL);
    }

    PathInfo *paths = (PathInfo*) args;
    struct stat stat_buffer;
    if (lstat(paths->src_path, &stat_buffer) == -1){
        perror("lstat error");
        destroyPaths(paths);
        pthread_exit(NULL);
    }
    if (S_ISDIR(stat_buffer.st_mode)){
        copy_directory(paths, stat_buffer.st_mode);
    }
    if (S_ISREG(stat_buffer.st_mode)){
        copy_regular_file(paths, stat_buffer.st_mode);
    }
    destroyPaths(paths);

    pthread_exit(NULL);
}

// we should save mode for directories and files to have permissions to use them
void copy_directory(PathInfo *paths, mode_t mode) {
    if (mkdir(paths->dest_path, mode) == -1){
        if (opendir(paths->dest_path) == NULL){
            perror("cant make destination dir");
            return;
        }

    }
    DIR *dir = opendir(paths->src_path);
    if (dir == NULL){
        perror("cant open source dir");
        return;
    }
    struct dirent *res_dirent;
    pthread_t pthread;
    while (true){
        res_dirent = readdir(dir);
        if (res_dirent == NULL){
            break;
        }
        if (strcmp(res_dirent->d_name, ".") == 0 || strcmp(res_dirent->d_name, "..") == 0){
            continue;
        }
        PathInfo *next_directory_paths = get_next_paths(paths->src_path, paths->dest_path, res_dirent->d_name);
        if (next_directory_paths == NULL){
            continue;
        }
        pthread_create(&pthread, NULL, copy_files, next_directory_paths);
    }
}

void copy_regular_file(PathInfo *paths, mode_t mode) {
    int source_fd = open(paths->src_path, O_RDONLY, mode);
    if (source_fd == 0){
        perror("open source file failed");
        exit(-1);
    }
    int destination_fd = open(paths->dest_path, O_WRONLY | O_CREAT, mode);
    if (destination_fd == -1){
        close(source_fd);
        perror("open destination file failed");
        exit(-1);
    }
    copy_file_data(source_fd, destination_fd);
    close(source_fd);
    close(destination_fd);
}

void copy_file_data(int source_fd, int dest_fd) {
    char buf[BUFSIZ];
    size_t read_bites;
    while((read_bites = read(source_fd, buf, BUFSIZ))){
        write(dest_fd, buf, read_bites);
    }
}

void destroyPaths(PathInfo *paths) {
    free(paths);
}

PathInfo *get_next_paths(const char *source, const char *destination, const char *next_file_name) {
    PathInfo *next_paths = calloc(1, sizeof(PathInfo));
    if (next_paths == NULL){
        perror("get_next_paths. cant calloc");
        return NULL;
    }
    char *delimiter = "/";

    next_paths->src_path = build_path(source, delimiter, next_file_name);
    next_paths->dest_path = build_path(destination, delimiter, next_file_name);
    return next_paths;
}


char *build_path(const char *last_directory_path, char *delimiter, const char *new_file_name) {
    size_t path_len = strlen(last_directory_path);
    size_t name_len = strlen(new_file_name);
    size_t delimiter_len = strlen(delimiter);
    char* new_path = (char*) malloc(path_len + name_len + delimiter_len + 1);
    if (new_path == NULL){
        perror("build_path. malloc failed");
        return NULL;
    }

    strcpy(new_path, last_directory_path);
    strcpy(new_path + path_len, delimiter);
    strcpy(new_path + path_len + delimiter_len, new_file_name);
    return new_path;
}
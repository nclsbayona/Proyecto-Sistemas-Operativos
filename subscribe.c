#include <stdlib.h>
typedef struct Subscribe{
    pid_t pid;
    char filename[100];
    char categories[100];
};

struct Subscribe *createSubscribe(pid_t pid, char *filename, char *categories){
    struct Subscribe *subscribe = malloc(sizeof(struct Subscribe));
    subscribe->pid = pid;
    strcpy(subscribe->filename, filename);
    strcpy(subscribe->categories, categories);
    return subscribe;
}
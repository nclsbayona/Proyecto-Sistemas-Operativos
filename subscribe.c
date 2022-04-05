#include <stdlib.h>
//Estructura de datos que almacena un PID, junto a un nombre de archivo y una lista de suscripciones
typedef struct Subscribe{
    pid_t pid;
    char filename[100];
    char categories[100];
};

//Crea un objeto suscripción a partir de un PID, un nombre de archivo y una lista de categorias
struct Subscribe *createSubscribe(pid_t pid, char *filename, char *categories){
    struct Subscribe *subscribe = malloc(sizeof(struct Subscribe));
    subscribe->pid = pid;
    strcpy(subscribe->filename, filename);
    strcpy(subscribe->categories, categories);
    return subscribe;
}
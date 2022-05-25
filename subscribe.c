#include <stdlib.h>
//Estructura de datos que almacena un PID, junto a un nombre de archivo y una lista de suscripciones
/**
 * Structure: Subscribe
 * Description: stores the pid, tha name of the file, and a list of categories
 */
typedef struct Subscribe{
    pid_t pid;
    char filename[100];
    char categories[100];
};

//Crea un objeto suscripción a partir de un PID, un nombre de archivo y una lista de categorias
/**
 * Constructor for the Subscribe structure
 */
struct Subscribe *createSubscribe(pid_t pid, char *filename, char *categories){
    struct Subscribe *subscribe = malloc(sizeof(struct Subscribe));
    subscribe->pid = pid;
    strcpy(subscribe->filename, filename);
    strcpy(subscribe->categories, categories);
    return subscribe;
}
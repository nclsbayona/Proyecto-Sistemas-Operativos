#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>
mode_t fifo_mode = S_IRUSR | S_IWUSR;
//Estructura de datos que almacena la información de un articulo de noticias
typedef struct NewsArticle
{
    char category;
    char text[100];
};

//Estructura de datos que almacena la información para envíar un articulo de noticias junto con el PID del proceso que lo envía
typedef struct Message
{
    pid_t id;
    struct NewsArticle article;
};

//Crea un objeto de tipo mensaje a partir de un articulo de noticias y un PID
struct Message *createMessage(pid_t id, struct NewsArticle article)
{
    struct Message *message = malloc(sizeof(struct Message));
    message->id = id;
    message->article = article;
    return message;
}

//Crea un objeto de tipo articulo de noticias a partir de una categoria y un texto
struct NewsArticle *createNewsArticle(char category, char *text)
{
    struct NewsArticle *article = malloc(sizeof(struct NewsArticle));
    article->category = category;
    bool e = false;
    for (int i = 0; i < strlen(text) && !e; ++i)
    {
        if (text[i] == '\0')
            e = true;
        article->text[i] = text[i];
    }
    return article;
}
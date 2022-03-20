#include "map.c"
#include "news.c"
typedef struct CommunicationSystem{
    struct Map* writingPipes;
    struct NewsArticle** articles;
    int len;
    // TODO: Guardar pipes de lectura
};

struct CommunicationSystem *createCommunicationSystem(){
    struct CommunicationSystem *cs = malloc(sizeof(struct CommunicationSystem));
    cs->articles = NULL;
    cs->len = 0;
    cs->writingPipes = createMap();
    return cs;
}

void addNewsArticle(struct CommunicationSystem *cs, char category, char *text){
    struct NewsArticle *article = createNewsArticle(category, text);
    if (cs->len++ > 0)
        cs->articles = realloc(cs->articles, sizeof(struct NewsArticle*) * cs->len);
    else
        cs->articles = malloc(sizeof(struct NewsArticle*));
    cs->articles[cs->len - 1] = article;
}
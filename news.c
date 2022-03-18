#include <stdlib.h>

typedef struct NewsArticle{
    char category;
    char *text;
};

struct NewsArticle *createNewsArticle(char category, char *text){
    struct NewsArticle *article = malloc(sizeof(struct NewsArticle));
    article->category = category;
    article->text = text;
    return article;
}
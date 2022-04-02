#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct NewsArticle
{
    char category;
    char text[100];
};

typedef struct Message
{
    pid_t id;
    struct NewsArticle article;
};

struct Message *createMessage(pid_t id, struct NewsArticle article)
{
    struct Message *message = malloc(sizeof(struct Message));
    message->id = id;
    message->article = article;
    return message;
}

int len(char *str)
{
    int i = 0;
    while (str[i] != '\0')
        i++;
    return i;
}

struct NewsArticle *createNewsArticle(char category, char *text)
{
    struct NewsArticle *article = malloc(sizeof(struct NewsArticle));
    article->category = category;
    bool e = false;
    for (int i = 0; i < len(text) && !e; ++i)
    {
        if (text[i] == '\0')
            e = true;
        article->text[i] = text[i];
    }
    return article;
}
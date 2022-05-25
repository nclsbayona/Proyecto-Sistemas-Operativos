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

/*
file: news.c
Authors: Nicolas Bayona, Manuel Rios, Abril Cano 
Contains: implementation of functions that creates the structures
of news that will be sent, managed, created etc
Date of last update: 24/05/2022
*/

mode_t fifo_mode = S_IRUSR | S_IWUSR;

/**
 * The type NewsArticle is a struct that contains a char and an array of 100 chars.
 * @property {char} category - The category of the news article.
 * @property {char} text - The text of the article.
 */
typedef struct NewsArticle
{
    char category;
    char text[100];
};

/**
 * A Message is a struct that contains a pid_t and a NewsArticle.
 * @property {pid_t} id - The process ID of the process that sent the message.
 * @property article - The article to be sent.
 */
typedef struct Message
{
    pid_t id;
    struct NewsArticle article;
};


/**
 * It creates a message
 * 
 * @param id The id of the process that sent the message
 * @param article The article to be sent.
 * 
 * @return A pointer to a struct Message.
 */
struct Message *createMessage(pid_t id, struct NewsArticle article)
{
    struct Message *message = malloc(sizeof(struct Message));
    message->id = id;
    message->article = article;
    return message;
}


/**
 * It creates a new news article
 * 
 * @param category The category of the news article.
 * @param text The text of the article.
 * 
 * @return A pointer to a NewsArticle struct.
 */
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
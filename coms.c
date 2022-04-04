#include "map.c"
#include "news.c"
typedef struct CommunicationSystem{
    struct Map* writingPipes;
    struct NewsArticle** articles;
    pid_t *ids;
    int len;
    int size_ids;
};

bool idFound(struct CommunicationSystem *cs, pid_t pid)
{
    for (int i = 0; i < cs->size_ids; i++)
        if (cs->ids[i] == pid)
            return true;
    return false;
}

bool artFound(struct CommunicationSystem *cs, struct NewsArticle *article)
{
    for (int i = 0; i < cs->len; i++)
        if (strcmp(cs->articles[i]->text ,article->text)==0 && strcmp(cs->articles[i]->category, article->category)==0)
            return true;
    return false;
}

bool addId(struct CommunicationSystem *cs, pid_t id)
{
    if (cs->size_ids++>0)
        cs->ids = realloc(cs->ids, sizeof(int)*cs->size_ids);
    else
        cs->ids = malloc(sizeof(int)*cs->size_ids);
    cs->ids[cs->size_ids-1] = id;
    return true;
}

struct CommunicationSystem *createCommunicationSystem(){
    struct CommunicationSystem *cs = malloc(sizeof(struct CommunicationSystem));
    cs->articles = NULL;
    cs->len = 0;
    cs->size_ids = 0;
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

void subscribeToTopic(struct CommunicationSystem *cs, char key, char *filename){
    subscribeToEntry(cs->writingPipes, key, filename);
}

bool sendToSub(struct NewsArticle* art, char *filename){
    int pipe = open(filename, O_WRONLY, fifo_mode);
    if (pipe == -1)
    {
        perror("Error abriendo el pipe");
        return false;
    }
    write(pipe, art, sizeof(struct NewsArticle));
    return true;
}

void sendToSubs(struct NewsArticle *article, struct CommunicationSystem *cs){
    struct Entry *entry = searchEntryInMap(cs->writingPipes, article->category);
    if (entry != NULL)
        for (int i = 0; i < entry->value->len; i++)
           while  (!sendToSub(article, entry->value->filenames[i]));
}
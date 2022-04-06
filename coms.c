#include "map.c"
#include "news.c"
// Estructura de datos que almacena la información del sistema de comunicaciones
typedef struct CommunicationSystem
{
    struct Map *writingPipes;      // El mapa con los pipes a los que debo escribir por categoria
    struct NewsArticle **articles; // Los articulos que he enviado o que tengo almacenados
    pid_t *ids;                    // Los ids de los procesos que estan suscriitos
    int len;                       // La longitud del array de articulos
    pid_t *idsP;                   // Los ids de los procesos publicadores
    int size_idsP;                 // La longitud del array de idsP
    int size_ids;                  // La longitud del array de ids
};

// Busca un id en el array de ids de un sistema de comunicaciones
bool idFound(struct CommunicationSystem *cs, pid_t pid)
{
    for (int i = 0; i < cs->size_ids; i++)
        if (cs->ids[i] == pid)
            return true;
    return false;
}

// Busca un id en el array de idsP de un sistema de comunicaciones
bool idPFound(struct CommunicationSystem *cs, pid_t pid)
{
    for (int i = 0; i < cs->size_idsP; i++)
        if (cs->idsP[i] == pid)
            return true;
    return false;
}

// Quita un id en el array de idsP de un sistema de comunicaciones
bool removeidP(struct CommunicationSystem *cs, pid_t pid)
{
    for (int i = 0; i < cs->size_idsP; i++)
        if (cs->idsP[i] == pid)
        {
            for (int j = i + 1; j < cs->size_idsP; j++)
                cs->idsP[j - 1] = cs->idsP[j];
            cs->size_idsP--;
            cs->idsP = realloc(cs->idsP, sizeof(int) * cs->size_idsP);
            return true;
        }
    return false;
}

// Busca un articulo en el array de articulos de un sistema de comunicaciones
bool artFound(struct CommunicationSystem *cs, struct NewsArticle *article)
{
    for (int i = 0; i < cs->len; i++)
        if (strcmp(cs->articles[i]->text, article->text) == 0 && cs->articles[i]->category == article->category)
            return true;
    return false;
}

// Añade un id al array de ids de un sistema de comunicaciones
bool addId(struct CommunicationSystem *cs, pid_t id)
{
    if (cs->size_ids++ > 0)
        cs->ids = realloc(cs->ids, sizeof(int) * cs->size_ids);
    else
        cs->ids = malloc(sizeof(int) * cs->size_ids);
    cs->ids[cs->size_ids - 1] = id;
    return true;
}

// Añade un id al array de idsP de un sistema de comunicaciones
bool addIdP(struct CommunicationSystem *cs, pid_t id)
{
    if (cs->size_idsP++ > 0)
        cs->idsP = realloc(cs->idsP, sizeof(int) * cs->size_idsP);
    else
        cs->idsP = malloc(sizeof(int) * cs->size_idsP);
    cs->idsP[cs->size_idsP - 1] = id;
    return true;
}

// Inicializa un sistema de comunicaciones
struct CommunicationSystem *createCommunicationSystem()
{
    struct CommunicationSystem *cs = malloc(sizeof(struct CommunicationSystem));
    cs->articles = NULL;
    cs->len = 0;
    cs->size_ids = 0;
    cs->size_idsP = 0;
    cs->writingPipes = createMap();
    return cs;
}

// Añade un articulo al array de articulos de un sistema de comunicaciones
void addNewsArticle(struct CommunicationSystem *cs, char category, char *text)
{
    struct NewsArticle *article = createNewsArticle(category, text);
    if (cs->len++ > 0)
        cs->articles = realloc(cs->articles, sizeof(struct NewsArticle *) * cs->len);
    else
        cs->articles = malloc(sizeof(struct NewsArticle *));
    cs->articles[cs->len - 1] = article;
}

// Suscribe a un proceso a un topico en especifico, añade el pipe de este a la lista de pipes de escritura para una categoria de un sistema de comunicaciones
void subscribeToTopic(struct CommunicationSystem *cs, char key, char *filename)
{
    subscribeToEntry(cs->writingPipes, key, filename);
}

// Envia un articulo a un suscriptor (Al pipe de este)
bool sendToSub(struct NewsArticle *art, char *filename)
{
    int pipe = open(filename, O_WRONLY, fifo_mode);
    if (pipe == -1)
    {
        perror("Error abriendo el pipe");
        return false;
    }
    write(pipe, art, sizeof(struct NewsArticle));
    return true;
}

// Envia una noticia a todos los suscriptores de una categoria de articulo
void sendToSubs(struct NewsArticle *article, struct CommunicationSystem *cs)
{
    struct Entry *entry = searchEntryInMap(cs->writingPipes, article->category);
    if (entry != NULL)
        for (int i = 0; i < entry->value->len; i++)
            while (!sendToSub(article, entry->value->filenames[i]))
                ;
}

// Envia una noticia a todos los suscriptores
void sendToAll(struct NewsArticle *article, struct CommunicationSystem *cs)
{

    for (int i = 0; i < cs->writingPipes->len; i++)
    {
        struct Entry *entry = cs->writingPipes->entries[i];
        for (int j = 0; j < entry->value->len; j++)
            while (!sendToSub(article, entry->value->filenames[j]))
                ;
    }
}
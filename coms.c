#include "map.c"
#include "news.c"
// Estructura de datos que almacena la información del sistema de comunicaciones
/**
 * `struct CommunicationSystem` is a struct that contains a `struct Map *`, a `struct NewsArticle **`,
 * a `pid_t *`, an `int`, a `pid_t *`, an `int`, and an `int`.
 * @property writingPipes - This is a map that contains the pipes that the process should write to. The
 * key is the category and the value is an array of pipes.
 * @property articles - This is an array of NewsArticle pointers. This is where the articles are
 * stored.
 * @property {pid_t} ids - This is an array of the ids of the processes that are subscribed to the
 * communication system.
 * @property {int} len - The length of the array of articles.
 * @property {pid_t} idsP - The array of ids of the publishers.
 * @property {int} size_idsP - The length of the array of idsP.
 * @property {int} size_ids - The length of the array of ids.
 */
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
/**
 * It checks if the pid is in the list of ids
 * 
 * @param cs The communication system
 * @param pid the process id of the process that sent the message
 * 
 * @return A boolean value.
 */
bool idFound(struct CommunicationSystem *cs, pid_t pid)
{
    for (int i = 0; i < cs->size_ids; i++)
        if (cs->ids[i] == pid)
            return true;
    return false;
}

// Busca un id en el array de idsP de un sistema de comunicaciones
/**
 * It checks if a given pid is in the list of pids
 * 
 * @param cs The communication system
 * @param pid the process id of the process that sent the message
 * 
 * @return A boolean value.
 */
bool idPFound(struct CommunicationSystem *cs, pid_t pid)
{
    for (int i = 0; i < cs->size_idsP; i++)
        if (cs->idsP[i] == pid)
            return true;
    return false;
}

// Quita un id en el array de idsP de un sistema de comunicaciones
/**
 * It removes a pid from the list of pids
 * 
 * @param cs the CommunicationSystem struct
 * @param pid the process id of the process to be removed
 * 
 * @return true if the pid was found and removed, false otherwise.
 */
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
/**
 * It checks if the article is already in the system
 * 
 * @param cs The communication system
 * @param article The article to check for.
 * 
 * @return A boolean value.
 */
bool artFound(struct CommunicationSystem *cs, struct NewsArticle *article)
{
    for (int i = 0; i < cs->len; i++)
        if (strcmp(cs->articles[i]->text, article->text) == 0 && cs->articles[i]->category == article->category)
            return true;
    return false;
}

// Añade un id al array de ids de un sistema de comunicaciones
/**
 * It adds a new id to the list of ids
 * 
 * @param cs The communication system
 * @param id The id of the process to add to the list of processes to communicate with.
 * 
 * @return A boolean value.
 */
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
/**
 * It adds a process id to the list of process ids
 * 
 * @param cs The CommunicationSystem struct
 * @param id The id of the process to add to the list of processes.
 * 
 * @return a boolean value.
 */
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
/**
 * It creates a communication system
 * 
 * @return A pointer to a CommunicationSystem struct.
 */
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
/**
 * It creates a new news article, adds it to the array of articles, and increases the length of the
 * array
 * 
 * @param cs A pointer to the CommunicationSystem struct.
 * @param category The category of the news article.
 * @param text The text of the article.
 */
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
/**
 * It subscribes a process to a topic
 * 
 * @param cs The communication system
 * @param key The key that the client will use to send messages to the server.
 * @param filename The name of the file to write to.
 */
void subscribeToTopic(struct CommunicationSystem *cs, char key, char *filename)
{
    subscribeToEntry(cs->writingPipes, key, filename);
}

// Envia un articulo a un suscriptor (Al pipe de este)
/**
 * It opens the pipe with the given filename, writes the article to it, and returns true if it was
 * successful
 * 
 * @param art The article to send.
 * @param filename The name of the pipe to send the article to.
 * 
 * @return A boolean value.
 */
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
/**
 * It sends the article to all the subscribers of the category
 * 
 * @param article The article to send to the subscribers.
 * @param cs The communication system
 */
void sendToSubs(struct NewsArticle *article, struct CommunicationSystem *cs)
{
    struct Entry *entry = searchEntryInMap(cs->writingPipes, article->category);
    if (entry != NULL)
        for (int i = 0; i < entry->value->len; i++)
            while (!sendToSub(article, entry->value->filenames[i]));
}

// Envia una noticia a todos los suscriptores
/**
 * It iterates through all the entries in the writingPipes hashtable, and for each entry, it iterates
 * through all the filenames in the entry's value, and sends the article to each of those filenames
 * 
 * @param article The article to send
 * @param cs The communication system
 */
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
#include "coms.c"
#include "subscribe.c"
#include <semaphore.h>

bool endL = false;
char *with_publishers;
char *with_subscriptors;
int timeP;
int fd, close_status, fd2, close_status2;
pthread_t thread_id1;
struct CommunicationSystem *cs;

sem_t s;

// Funcion que finaliza el SC, se añadio para que el SC finalize correctamente
/**
 * It kills all the processes that are still alive
 */
void end()
{
    unlink(with_publishers);
    unlink(with_subscriptors);
    int i;
    for (i = 0; i < cs->size_ids; i++)
        kill(cs->ids[i], SIGINT);
    for (i = 0; i < cs->size_idsP; i++)
        kill(cs->idsP[i], SIGINT);
    exit(0);
}

// Funcion para inicializar el SC, se añadio para poder inicializar el SC
/**
 * It creates the two pipes that will be used to communicate with the publishers and the subscriptors,
 * and it opens the pipe that will be used to communicate with the publishers
 * 
 * @param argc The number of arguments passed to the program.
 * @param argv the arguments passed to the program
 */
void startSystem(int argc, char **argv)
{
    cs = createCommunicationSystem();
    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            with_publishers = argv[i + 1];
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            with_subscriptors = argv[i + 1];
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            timeP = atoi(argv[i + 1]);
        }
    }
    int pipe;
    do
    {
        pipe = mkfifo(with_publishers, fifo_mode);
        if (pipe == -1)
        {
            perror("Error creando el pipe con publicadores");
            unlink(with_publishers);
            printf("Se volvera a intentar\n");
            sleep(timeP);
        }
    } while (pipe == -1);
    do
    {
        pipe = mkfifo(with_subscriptors, fifo_mode);
        if (pipe == -1)
        {
            perror("Error creando el pipe con suscriptores");
            unlink(with_subscriptors);
            printf("Se volvera a intentar\n");
            sleep(timeP);
        }
    } while (pipe == -1);
    do
    {
        fd = open(with_publishers, O_RDONLY);
        if (fd == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep((int)(timeP / 2));
        }
    } while (fd == -1);
}

// Funcion que envia un articulo a los suscriptores, se añadio para enviar un articulo a suscriptores
/**
 * It sends the article to all the subscribers in the cs list
 * 
 * @param article The article to send to subscribers.
 */
void sendArticle(struct NewsArticle *article)
{
    sendToSubs(article, cs);
}

// Funcion que lee un articulo, se añadio para leer un articulo
//TODO send multiple articles
/**
 * It reads a message from the pipe, checks if the process that sent the message is registered in the
 * list of processes, if it is not, it adds it to the list, then it checks if the article is already in
 * the list of articles, if it is not, it checks if the category is 0, if it is, it removes the process
 * from the list of processes, if the list of processes is empty, it waits for a certain amount of
 * time, if the list is still empty, it sends a message to all the processes that there are no more
 * articles, if the list is not empty, it checks if the category is not 0, if it is not, it prints the
 * article, adds it to the list of articles and sends it to all the processes
 */
void readArticle()
{
    struct Message *message = malloc(sizeof(struct Message));
    read(fd, message, sizeof(struct Message));
    struct NewsArticle *article = &(message->article);
    pid_t pid = message->id;
    if (!idPFound(cs, pid))
        addIdP(cs, pid);
    if (!artFound(cs, article))
    {
        if (article->category == 0)
        {
            removeidP(cs, pid);
        }
        if (cs->size_idsP == 0)
        {
            sleep(timeP);
            if (cs->size_idsP == 0)
            {
                article = createNewsArticle('.', "No hay mas articulos");
                sendToAll(article, cs);
                end();
            }
        }
        else if (article->category != 0)
        {
            printf("\nArticle\n%c: %s\n\n", article->category, article->text);
            sem_wait(&s);
            addNewsArticle(cs, article->category, article->text);
            sem_post(&s);
            sendArticle(article);
        }
    }
    free(message);
}

// Funcion que envia los articulos previamente escritos a los suscriptores, se añadio para enviar los articulos previamente escritos

/**
 * It sends all the articles that have the same category as the key to the subscriber
 * 
 * @param cs The CommunicationSystem struct.
 * @param key The category of the article.
 * @param filename The name of the file to which the article is to be sent.
 */
void sendPreviousArticles(struct CommunicationSystem *cs, char key, char *filename){
    sem_wait(&s);
    for(int i = 0; i < cs->len; i++){
        //printf("\nArticle\n%c: %c\n\n", key, cs->articles[i]->category);
        if(cs->articles[i]->category == key){
            /* Sending the article to the subscriber. */
            sendToSub(cs->articles[i],filename);
        }
    }
    sem_post(&s);
}

// Funcion que lee suscripciones permanentemente, se añadio para leer suscripciones permanentemente
/* It's a function that listens for subscriptors and adds them to the list of subscriptors. */
void listenForSubscriptors()
{
    do
    {
        fd2 = open(with_subscriptors, O_RDONLY, fifo_mode);
        if (fd2 == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep(timeP);
        }
    } while (fd2 == -1);
    while (true)
    {
        struct Subscribe *subscribe = malloc(sizeof(struct Subscribe));
        read(fd2, subscribe, sizeof(struct Subscribe));
        if (subscribe->pid != 0 && !idFound(cs, subscribe->pid))
        {
            addId(cs, subscribe->pid);
            for (int i = 0; i < strlen(subscribe->categories); i++)
            {
                char cat = subscribe->categories[i];
                if (cat != '\0' && cat != '_')
                    subscribeToTopic(cs, cat, subscribe->filename);
                    sendPreviousArticles(cs,cat,subscribe->filename);
            }
        }
    }
}

// Funcion para leer articulos permanentemente, se añadio para leer articulos permanentemente
/**
 * It writes "END" to the standard output and then calls the end() function
 */
void readTrue()
{
    while (true)
        readArticle();
}

// Funcion para capturar una señal y poder finalizar el SC correctamente, se añadio para poder capturar una señal y finalizar el SC
/**
 * It writes "END" to the standard output and then calls the end() function
 */
void catch_sigint()
{
    write(STDOUT_FILENO, "END", 4);
    end();
}

// Funcion para capturar una señal y poder finalizar el SC correctamente, se añadio para poder capturar una señal y finalizar el SC
/**
 * It writes "TERMINATE" to the standard output and then calls the end() function
 */
void catch_sigterm()
{
    write(STDOUT_FILENO, "TERMINATE", 10);
    end();
}

/**
 * It's a function that reads the arguments and starts the system
 * 
 * @param argc The number of arguments passed to the program.
 * @param argv 
 * 
 * @return The return value of the function is the exit status of the program.
 */
int main(int argc, char **argv)
{
    signal(SIGINT, catch_sigint);
    signal(SIGSTOP, catch_sigint);
    signal(SIGABRT, catch_sigterm);
    signal(SIGQUIT, catch_sigterm);
    signal(SIGTERM, catch_sigterm);

    sem_init(&s, 0, 1);

    if (argc < 7) {
        printf("Sintaxis invalida, revise los argumentos ingresados\n");
    }

    for (int i = 1; i < argc; i = i + 2){
        if (strcmp("-p", argv[i]) != 0 && strcmp("-s", argv[i]) != 0 && strcmp("-t", argv[i]) != 0){
            printf("Sintaxis invalida, revise los argumentos ingresados\n");
            return -1;
        }
    }

    startSystem(argc, argv);
    printf(" -p %s\n", with_publishers);
    printf(" -s %s\n", with_subscriptors);
    printf(" -t %d\n", timeP);
    pthread_create(&thread_id1, NULL, (void *)(listenForSubscriptors), NULL);
    readTrue();
    return 0;
}
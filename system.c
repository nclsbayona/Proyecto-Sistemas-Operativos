#include "coms.c"
#include "subscribe.c"
bool endL = false;
char *with_publishers;
char *with_subscriptors;
int timeP;
int fd, close_status, fd2, close_status2;
pthread_t thread_id1;
struct CommunicationSystem *cs;

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

void sendArticle(struct NewsArticle *article)
{
    sendToSubs(article, cs);
}

void readArticle()
{
    struct Message *message = malloc(sizeof(struct Message));
    read(fd, message, sizeof(struct Message));
    struct NewsArticle *article = &(message->article);
    pid_t pid = message->id;
    if (!idFound(cs, pid))
        addId(cs, pid);
    if (article->text[0] != '\0' && article->category != '\0' && !artFound(cs, article))
    {
        printf("%c: %s\n", article->category, article->text);
        addNewsArticle(cs, article->category, article->text);
        sendArticle(article);
    }
}

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
            }
        }
    }
}

void readTrue()
{
    while (true)
        readArticle();
}

void end()
{
    unlink(with_publishers);
    unlink(with_subscriptors);
    for (int i = 0; i < cs->len; i++)
        kill(cs->ids[i], SIGINT);
    exit(0);
}

void catch_sigint()
{
    write(STDOUT_FILENO, "END", 4);
    end();
}

void catch_sigterm()
{
    write(STDOUT_FILENO, "TERMINATE", 10);
    end();
}

int main(int argc, char **argv)
{
    signal(SIGINT, catch_sigint);
    signal(SIGTERM, catch_sigterm);
    startSystem(argc, argv);
    printf(" -p %s\n", with_publishers);
    printf(" -s %s\n", with_subscriptors);
    printf(" -t %d\n", timeP);
    pthread_create(&thread_id1, NULL, (void *)(listenForSubscriptors), NULL);
    readTrue();
    return 0;
}
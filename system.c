#include <string.h>
#include "coms.c"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
bool endL = false;
char *with_publishers;
char *with_subscriptors;
int time;
int fd, close_status;
mode_t fifo_mode = S_IRUSR | S_IWUSR;
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
            time = atoi(argv[i + 1]);
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
            sleep(time);
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
            sleep(time);
        }
    } while (pipe == -1);
    do
    {
        fd = open(with_publishers, O_RDONLY);
        if (fd == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep((int)(time / 2));
        }
    } while (fd == -1);
}

void sendArticle(struct NewsArticle *article)
{
    
}

void readArticle()
{
    struct Message *message=malloc(sizeof(struct Message));
    read(fd, message, sizeof(struct Message));
    struct NewsArticle *article=&(message->article);
    pid_t pid=message->id;
    if (!idFound(cs, pid))
        addId(cs, pid);
    if (article->text[0]!='\0' && article->category != '\0' && !artFound(cs, article))
    {
        printf("%c: %s\n", article->category, article->text);
        addNewsArticle(cs, article->category, article->text);
        sendArticle(article);
    }
}

void end()
{
    unlink(with_publishers);
    unlink(with_subscriptors);
    for (int i = 0; i < cs->len; i++)
        kill(cs->ids[i], SIGKILL);
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
    printf(" -t %d\n", time);
    while (true)
        readArticle();
    return 0;
}
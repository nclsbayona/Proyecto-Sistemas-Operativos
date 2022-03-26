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
    char category = article->category;
    struct Entry *entry = searchEntryInMap(cs->writingPipes, &category);
    if (entry)
        for (int i = 0; i < entry->value->len; i++)
        {
            int fd;
            do
            {
                fd = open(entry->value->filenames[i], O_WRONLY);
                if (fd == -1)
                {
                    perror("Error abriendo el pipe");
                    printf("Se volvera a intentar\n");
                    sleep(time);
                }
            } while (fd == -1);
            // write(fd, article, sizeof(struct NewsArticle));
            fd = close(fd);
        }
}

bool writeArticle(struct NewsArticle *article)
{
    write(fd, article, sizeof(struct NewsArticle));
    return true;
}

bool artFound(struct CommunicationSystem *cs, struct NewsArticle *article)
{
    for (int i = 0; i < cs->len; i++)
        if (strcmp(cs->articles[i]->text ,article->text)==0)
            return true;
    return false;
}

void readArticle()
{
    struct NewsArticle *article;
    article = malloc(sizeof(struct NewsArticle));
    read(fd, article, sizeof(struct NewsArticle));
    if (article->text[0]!='\0' && article->category != '\0' && !artFound(cs, article))
    {
        printf("%c: %s\n", article->category, article->text);
        addNewsArticle(cs, article->category, article->text);
        sendArticle(article);
    }
    free(article);
}

void end()
{
    unlink(with_publishers);
    unlink(with_subscriptors);
}

void catch_sigterm()
{
    write(STDOUT_FILENO, "END\n", 4);
    end();
}

int main(int argc, char **argv)
{
    startSystem(argc, argv);
    printf(" -p %s\n", with_publishers);
    printf(" -s %s\n", with_subscriptors);
    printf(" -t %d\n", time);
    signal(SIGINT, catch_sigterm);
    while (true)
        readArticle();
    return 0;
}
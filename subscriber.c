#include "coms.c"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
char *with_system;

void printArticle(struct NewsArticle *article)
{
    printf("New Article:\n%c: %s\n\n", article->category, article->text);
}

void startSystem(int argc, char **argv)
{
    for (int i = 1; i < argc; i += 2)
        if (strcmp(argv[i], "-s") == 0)
            with_system = argv[i + 1];
}

struct NewsArticle *readArticle()
{
    int fd, close_status;
    struct NewsArticle *article;
    do
    {
        fd = open(with_system, O_RDONLY);
        if (fd == -1)
        {
            const int time = 5;
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep(time);
        }
        else
        {
            printf("Reading article\n");
            article = malloc(sizeof(struct NewsArticle));
            read(fd, article, sizeof(struct NewsArticle));
            printf("Article read\n");
            printArticle(article);
            do
            {
                close_status = close(fd);
                if (close_status == -1)
                {
                    perror("close");
                    printf("Se volvera a intentar\n");
                }
            } while (close_status == -1);
        }
    } while (fd == -1);

    return article;
}

void fetchNew()
{
    printf ("Fetching new articles...\n");
    int i=0;
    while (i++<=3){
    struct NewsArticle *article = readArticle(with_system);
    if (article != NULL)
        printArticle(article);
    free(article);
    }
}

void end(){
    exit(0);
}

int main(int argc, char **argv)
{
    startSystem(argc, argv);
    fetchNew();
    end();
    return 0;
}
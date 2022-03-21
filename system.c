#include "coms.c"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

char* with_publishers;
char* with_subscriptors;
int time;
struct CommunicationSystem *cs;

void startSystem(int argc, char **argv){
    for (int i=1; i<argc; i+=2){
        if (strcmp(argv[i], "-p") == 0){
            with_publishers = argv[i+1];
        }
        else if (strcmp(argv[i], "-s") == 0){
            with_subscriptors = argv[i+1];
        }
        else if (strcmp(argv[i], "-t") == 0){
            time = atoi(argv[i+1]);
        }
    }
}

void readArticle(){
    int fd, close_status;
    struct NewsArticle *article;
    do
    {
        fd = open(with_publishers, O_RDONLY);
        if (fd == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep((int)(time/2));
        }
        else
        {
            article = malloc(sizeof(struct NewsArticle));
            read(fd, article, sizeof(struct NewsArticle));
            do{
                close_status = close(fd);
                if (close_status == -1)
                {
                    perror("close");
                    printf("Se volvera a intentar\n");
                }
            }while (close_status == -1);
        }
    } while (fd == -1);
    printf("New Article:\n%c: %s\n\n", article->category, article->text);
    addNewsArticle(cs, article->category, article->text);
    free(article);
}

int main(int argc, char **argv)
{
    startSystem(argc, argv);
    cs=createCommunicationSystem();
    printf(" -p %s\n", with_publishers);
    printf(" -s %s\n", with_subscriptors);
    printf(" -t %d\n", time);
    readArticle();
    free(cs);
    return 0;
}

#include <string.h>
#include "coms.c"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
char* with_publishers;
char* with_subscriptors;
int time;
mode_t fifo_mode = S_IRUSR | S_IWUSR;
struct CommunicationSystem *cs;

void startSystem(int argc, char **argv){
    cs=createCommunicationSystem();
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
    int pipe;
    do
    {
        pipe = mkfifo(with_publishers, fifo_mode);
        if (pipe == -1)
        {
            perror("Error creando el pipe");
            printf("Se volvera a intentar\n");
            sleep(time);
        }
    } while (pipe == -1);
    do
    {
        pipe = mkfifo(with_subscriptors, fifo_mode);
        if (pipe == -1)
        {
            perror("Error creando el pipe");
            printf("Se volvera a intentar\n");
            sleep(time);
        }
    } while (pipe == -1);
}

bool writeArticle(struct NewsArticle *article, char *filename)
{
    int fd, close_status;
    do
    {
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, fifo_mode);
        if (fd == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep(time);
        }
        else
        {
            write(fd, article, sizeof(struct NewsArticle));
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
    return true;
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
    writeArticle(article, with_subscriptors);
    free(article);
}

void end(){
    unlink(with_publishers);
    unlink (with_subscriptors);
    free(cs);
}

int main(int argc, char **argv){
    startSystem(argc, argv);
    printf(" -p %s\n", with_publishers);
    printf(" -s %s\n", with_subscriptors);
    printf(" -t %d\n", time);
    readArticle();
    end();
    return 0;
}
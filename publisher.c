#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include "news.c"

char* with_system;
char* archive;
int time;

bool writeArticle(struct NewsArticle *article, char *filename)
{
    int fd, close_status;
    do
    {
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
        if (fd == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
        }
        else
        {
            write(fd, article, sizeof(struct NewsArticle));
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
    return true;
}

void startSystem(int argc, char **argv){
    for (int i=1; i<argc; i+=2){
        if (strcmp(argv[i], "-p") == 0){
            with_system = argv[i+1];
        }
        else if (strcmp(argv[i], "-f") == 0){
            archive = argv[i+1];
        }
        else if (strcmp(argv[i], "-t") == 0){
            time = atoi(argv[i+1]);
        }
    }
}

int main(int argc, char **argv)
{
    startSystem(argc, argv);
    printf(" -p %s\n", with_system);
    printf(" -s %s\n", archive);
    printf(" -t %d\n", time);
    return 0;
}
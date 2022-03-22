#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include "news.c"

char *with_system;
char *archive;
int time;
mode_t fifo_mode= S_IRUSR | S_IWUSR;

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

void readArticles()
{
    FILE *fd;
    while (true)
    {
        do
        {
            fd = fopen(archive, "r");
            if (fd == NULL)
            {
                perror("Error abriendo el archivo para leer");
                printf("Se volvera a intentar\n");
                sleep((int)(time / 2));
            }
            while (!feof(fd))
            {
                char category;
                char text[100];
                fscanf(fd, "%c %s", &category, text);
                if (category != '\0' && text[0] != '\0')
                {
                    struct NewsArticle *article = createNewsArticle(category, text);
                    writeArticle(article, with_system);
                    printf("New Article:\n%c: %s\n\n", article->category, article->text);
                    free(article);
                }
            }
            fclose(fd);
        } while (fd == NULL);
    }
}

void startSystem(int argc, char **argv)
{
    int pipe=mkfifo(with_system, fifo_mode);
    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-p") == 0)
            with_system = argv[i + 1];
        else if (strcmp(argv[i], "-s") == 0)
            archive = argv[i + 1];
        else if (strcmp(argv[i], "-t") == 0)
            time = atoi(argv[i + 1]);        
    }
}

bool createArticle(char category, char *text)
{
    struct NewsArticle *article = createNewsArticle(category, text);
    if (writeArticle(article, with_system))
    {
        free(article);
        return true;
    }
    else
    {
        free(article);
        return false;
    }
}

void exit(){
    unlink (with_system);
    remove (archive);
    free(with_system);
    free(archive);
    exit(0);
}

int main(int argc, char **argv)
{
    startSystem(argc, argv);
    printf(" -p %s\n", with_system);
    printf(" -s %s\n", archive);
    printf(" -t %d\n", time);

    char category;
    char *text;

    while (true)
    {
        readArticles();
        /*
        printf("Introduzca la categoria del articulo: ");
        scanf("%c", &category);
        printf("Introduzca el texto del articulo: ");
        scanf("%s", text);
        if (createArticle(category, text))
            printf("Articulo creado correctamente\n");
        else
            printf("No se ha podido crear el articulo\n");
        */
    }
    return 0;
}
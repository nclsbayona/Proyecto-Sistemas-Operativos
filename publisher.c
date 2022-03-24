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
mode_t fifo_mode = S_IRUSR | S_IWUSR;

void createFile(char *name)
{
    int fd = open(name, O_RDWR | O_CREAT, fifo_mode);
    close(fd);
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

void readArticles()
{
    FILE *fd = NULL;
    do
    {
        fd = fopen(archive, "r");
        if (fd == NULL)
        {
            perror("Error abriendo el archivo para leer");
            printf("Se volvera a intentar\n");
            createFile(archive);
            sleep((int)(time / 2));
        }
    } while (fd == NULL);
    while (!feof(fd))
    {
        char category;
        char text[100];
        fscanf(fd, "%c: %s", &category, text);
        printf ("'%c' '%s'\n", category, text);
        if (category != '\0' && text[0] != '\0')
        {
            struct NewsArticle *article = createNewsArticle(category, text);
            printf("New Article:\n%c: %s\n\n", article->category, article->text);
            writeArticle(article, with_system);
            free(article);
        }
    }
    fclose(fd);
}

void startSystem(int argc, char **argv)
{
    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-p") == 0)
            with_system = argv[i + 1];
        else if (strcmp(argv[i], "-f") == 0)
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

void end()
{
    exit(0);
}

int main(int argc, char **argv)
{
    startSystem(argc, argv);
    printf(" -p %s\n", with_system);
    printf(" -f %s\n", archive);
    printf(" -t %d\n", time);
    /*char category;
    char *text;*/
    readArticles();
    end();
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
    return 0;
}
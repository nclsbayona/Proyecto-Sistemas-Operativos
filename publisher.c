#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include "news.c"
#include <signal.h>
#include <pthread.h>
int fd, f, close_status;
char *with_system;
char *archive;
int timeP;
mode_t fifo_mode = S_IRUSR | S_IWUSR;
pthread_t thread_id1;
void createFile(char *name)
{
    int fd = open(name, O_RDWR | O_CREAT, fifo_mode);
    close(fd);
}

bool writeArticle(struct NewsArticle article, char *filename){
    write(fd, createMessage(getpid(), article), sizeof(struct Message));
    return true;
}

void deleteLine(FILE *srcFile, FILE *tempFile, const int line)
{
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    int count = 0;
    while ((fgets(buffer, BUFFER_SIZE, srcFile)) != NULL)
    {
        if (line != count++)
            fputs(buffer, tempFile);
    }
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
            sleep((int)(timeP / 2));
        }
    } while (fd == NULL);
    char category;
    char text[100];
    if (fscanf(fd, "%c: %s", &category, text) > 0)
        if (category != '\0' && text[0] != '\0' && strlen(text) > 0)
        {
            struct NewsArticle article = *createNewsArticle(category, text);
            printf("\nNew Article:\n%c: %s\n\n", category, text);
            writeArticle(article, with_system);
        }
    FILE *tempFile = fopen("delete-line.tmp", "w");
    rewind(fd);
    deleteLine(fd, tempFile, 0);
    fclose(fd);
    remove(archive);
    fclose(tempFile);
    rename("delete-line.tmp", archive);
    sleep(2);
    int w = 0;
    do
    {
        fd = fopen(archive, "r");
        if (fd == NULL)
        {
            perror("Error abriendo el archivo para volver a leer");
            printf("Se volvera a intentar\n");
            sleep((int)(timeP / 2));
        }
    } while (fd == NULL && ++w < 3);
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
            timeP = atoi(argv[i + 1]);
    }
    remove("delete-line.tmp");
    do
    {
        fd = open(with_system, O_WRONLY, fifo_mode);
        if (fd == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep(timeP);
        }
    } while (fd == -1);
}

bool createArticle(char category, char *text)
{
    struct NewsArticle *article = createNewsArticle(category, text);
    if (writeArticle(*article, archive))
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
    remove("delete-line.tmp");
    pthread_exit(NULL);
    do
    {
        close_status = close(fd);
        if (close_status == -1)
        {
            perror("close");
        }
    } while (close_status == -1);
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

void readTrue()
{
    while (true)
        readArticles();
}

void readSTDIN()
{
    int i=0;
    while (true)
    {
        char category;
        char *text = malloc(sizeof(char) * 90);
        if (i++)
        scanf("%c", &category);
        printf("Introduzca la categoria del articulo: ");
        scanf("%c", &category);
        printf("Introduzca el texto del articulo: ");
        scanf("%s", text);
        ;
        if (createArticle(category, text))
            printf("Articulo creado correctamente\n");
        else
            printf("No se ha podido crear el articulo\n");
        free(text);
    }
}

int main(int argc, char **argv)
{
    signal(SIGINT, catch_sigint);
    signal(SIGTERM, catch_sigterm);
    startSystem(argc, argv);
    printf(" -p %s\n", with_system);
    printf(" -f %s\n", archive);
    printf(" -t %d\n", timeP);
    pthread_create(&thread_id1, NULL, (void *)(readTrue), NULL);
    readSTDIN();
    return 0;
}
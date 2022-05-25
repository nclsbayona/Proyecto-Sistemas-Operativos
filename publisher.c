#include "news.c"
int fd, f, close_status;
char *with_system;
char *archive;
int timeP;
pthread_t thread_id1;

/*
file: publisher.c
Authors: Nicolas Bayona, Manuel Rios, Abril Cano 
Contains: implementation of functions that creates the structures
of news that will be sent, managed, created etc
Date of last update: 24/05/2022
*/

/**
 * Function: createFile
 * 
 * Parameters: name The name of the file to create.
 * Returns: None
 * Description: It creates a file with the name passed in as the first argument.
 */
void createFile(char *name)
{
    int fd = open(name, O_RDWR | O_CREAT, fifo_mode);
    close(fd);
}


/**
 * Function: writeArticle
 * 
 * Parameters: article The article to be written.
 *           filename The name of the file to write to.
 * 
 * Returns: A boolean value.
 * Decription: It writes an article to a file
 */
bool writeArticle(struct NewsArticle article, char *filename)
{
    printf("Enviando articulo\n");
    write(fd, createMessage(getpid(), article), sizeof(struct Message));
    sleep(timeP);
    printf("Articulo enviado\n");
    return true;
}


/**
 * Function: end
 * Parameters: None
 * Returns: None
 * Description: It sends a message to the server to tell it to close the file descriptor
 */
void end()
{
    remove("delete-line.tmp");
    struct NewsArticle *art = createNewsArticle((char)0, "END");
    write(fd, createMessage(getpid(), *art), sizeof(struct Message));
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


/**
 * Function: deleteLine 
 *
 * Parameters: srcFile The file you want to delete a line from.
 *           tempFile The temporary file that will be used to store the contents of the source file, minus the 
 *           line that is to be deleted.
 *           line The line number to delete.
 * Returns: None
 * Description: It reads the source file line by line, and writes the lines to the temporary file, except for the
                  line to be deleted
 */
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

/**
 * Function: startSystem
 * 
 * Parameters: argc number of arguments
 *              argv -p /tmp/fifo -f /tmp/log.txt -t 5
 * Returns: None
 * Description: It opens a pipe to the system and if it fails, it will try again after a certain amount of time
 */
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


/**
 * Function: createArticle
 * 
 * Parameters: category The category of the article.
 *              text The text of the article.
 * 
 * Returns: A pointer to a struct NewsArticle.
 * Description: It creates a new article, writes it to the archive, and then frees the article
 */
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


/**
 * Function: readArticles
 * Parameters: None
 * Returns: None
 *
 * Description: 
 * It reads the first line of a file, creates an article with the information in the line, and then
 * deletes the line from the file.
 */
void readArticles()
{
    FILE *fd = NULL;
    do
    {
        fd = fopen(archive, "r");
        if (fd == NULL)
        {
            perror("Error abriendo el archivo para leer");
            // printf("Se volvera a intentar\n");
            createFile(archive);
            sleep((int)(timeP / 2));
        }
    } while (fd == NULL);
    char *category = malloc(sizeof(char));
    char *text = malloc(sizeof(char) * 100);
    fscanf(fd, "%c: %s", category, text);
    if (*category == 0)
    {
        printf("\nArchivo vacio\n");
        free(category);
        fclose(fd);
        end();
    }
    else if (*category != (char)10 && text[0] != 0 && text[0] != (char)10 && strlen(text) > 1)
    {

        printf("\nNew Article:\n%c: %s\n\n", *category, text);
        createArticle(*category, text);
    }
    free(category);
    free(text);
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
            // printf("Se volvera a intentar\n");
            createFile(archive);
            sleep((int)(timeP / 2));
        }
    } while (fd == NULL && ++w < 3);
    fclose(fd);
}

/**
 * Function: catch_sigint
 * Parameters: None
 * Returns: None
 * Description: It sets up a signal handler for SIGINT (Ctrl-C) that will call the end() function
 */
void catch_sigint()
{
    write(STDOUT_FILENO, "END", 3);
    end();
}


/**
 * Function: catch_sigterm
 * Parameters: None
 * Return: None
 * Description:
 * It sets up a signal handler for SIGTERM, which is the signal sent to a process when it is to be
 * terminated
 */
void catch_sigterm()
{
    write(STDOUT_FILENO, "TERMINATE", 9);
    end();
}


/**
 *
 * Function: catch_alarm
 * Parameters: None
 * Returns: None 
 * Description: The function `catch_alarm()` is called when the alarm goes off.
 */
void catch_alarm()
{
    write(STDOUT_FILENO, "ALARM", 5);
}


/**
 * Function: readTrue
 * Parameters: None
 * Returns: None 
 * Description:It reads articles until it reads a true article
 */
void readTrue()
{
    while (true)
        readArticles();
}


/**
 * Function: readSTDIN
 * Parameters: None
 * Returns: None 
 * Description:It reads from stdin and creates an article
 */
void readSTDIN()
{
    int i = 0;
    while (true)
    {
        char category;
        char *text = malloc(sizeof(char) * 90);
        if (i++)
            scanf("%c", &category);
        printf("Introduzca la categoria del articulo: ");
        scanf("%c", &category);
        printf("Introduzca el texto del articulo (Maximo 80 caracteres): ");
        scanf("%s", text);
        if (createArticle(category, text))
            printf("\nArticulo creado correctamente\n");
        else
            printf("\nNo se ha podido crear el articulo\n");
        free(text);
    }
}

/**
 * 
 * Function: Main
 * Parameters: argc number of arguments
 *             argv -p /bin/ls -f /home/user/Documents/file.txt -t 5
 * 
 * Return: The return value of the function is the exit status of the child process.
 * Description: It reads the arguments from the command line and starts the system
 */
int main(int argc, char **argv)
{
    signal(SIGALRM, catch_alarm);
    signal(SIGINT, catch_sigint);
    signal(SIGTERM, catch_sigterm);

    if (argc < 7) {
        printf("Sintaxis invalida, revise los argumentos ingresados\n");
    }

    for (int i = 1; i < argc; i = i + 2){
        if (strcmp("-p", argv[i]) != 0 && strcmp("-f", argv[i]) != 0 && strcmp("-t", argv[i]) != 0){
            printf("Sintaxis invalida, revise los argumentos ingresados\n");
            return -1;
        }
    }

    startSystem(argc, argv);
    printf(" -p %s\n", with_system);
    printf(" -f %s\n", archive);
    printf(" -t %d\n", timeP);
    pthread_create(&thread_id1, NULL, (void *)(readTrue), NULL);
    readSTDIN();
    return 0;
}
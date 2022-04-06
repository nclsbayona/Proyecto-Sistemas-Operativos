#include "news.c"
int fd, f, close_status;
char *with_system;
char *archive;
int timeP;
pthread_t thread_id1;

// Funcion que crea un archivo a partir de un nombre, se añadio para poder crear un archivo en caso de que no exista
void createFile(char *name)
{
    int fd = open(name, O_RDWR | O_CREAT, fifo_mode);
    close(fd);
}

// Funcion que envia un articulo a un pipe especificado (Para el pipe de escritura), se añadio para poder enviar un articulo al SC
bool writeArticle(struct NewsArticle article, char *filename)
{
    printf("Enviando articulo\n");
    write(fd, createMessage(getpid(), article), sizeof(struct Message));
    sleep(timeP);
    printf("Articulo enviado\n");
    return true;
}

// Funcion que finaliza el publicador, se añadio para poder finalizar un publicador
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

// Funcion que elimina una linea de un archivo y guarda el contenido en otro, se añadio para poder eliminar una linea de un archivo al leer de este
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

// Funcion que inicializa lo relacionado con un publicador, se añadio para poder inicializar un publicador
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

// Funcion que crea un articulo y lo envia a un pipe especificado (Otra funcion), se añadio para poder crear un articulo y enviarlo al SC
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

// Funcion que lee articulos de un archivo y los envia a un pipe especificado (Otra función), se añadio para poder leer articulos de un archivo y enviarlos al SC
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
    printf("category %d\n", *category);
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

// Funcion para capturar una señal y poder finalizar el publicador correctamente, se añadio para poder capturar una señal y finalizar el publicador
void catch_sigint()
{
    write(STDOUT_FILENO, "END", 3);
    end();
}

// Funcion para capturar una señal y poder finalizar el publicador correctamente, se añadio para poder capturar una señal y finalizar el publicador
void catch_sigterm()
{
    write(STDOUT_FILENO, "TERMINATE", 9);
    end();
}

// Funcion para capturar una señal de alarma, se añadio para poder capturar una señal de alarma y seguir con la ejecucion del programa
void catch_alarm()
{
    write(STDOUT_FILENO, "ALARM", 5);
}

// Funcion para leer el archivo eternamente, se añadio para poder leer el archivo
void readTrue()
{
    while (true)
        readArticles();
}

// Funcion para leer la entrada de texto
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

int main(int argc, char **argv)
{
    signal(SIGALRM, catch_alarm);
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
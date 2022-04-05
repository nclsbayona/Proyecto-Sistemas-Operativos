#include "news.c"
#include "subscribe.c"
int pipeS, fd, close_status;
char *with_system;
char pipename[100];
pthread_t thread_id1;

//Funcion para imprimir un articulo, se añadio para imprimir un articulo
void printArticle(struct NewsArticle *article)
{
    printf("\nNew Article:\n%c: %s\n\n", article->category, article->text);
}

//Funcion que inicializa lo relacionado con un suscriptor, se añadio para poder inicializar un suscriptor
void startSystem(int argc, char **argv)
{
    for (int i = 1; i < argc; i += 2)
        if (strcmp(argv[i], "-s") == 0)
            with_system = argv[i + 1];
    do
    {
        fd = open(with_system, O_WRONLY, fifo_mode);
        if (fd == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep(5);
        }
    } while (fd == -1);
    sprintf(pipename, "PipeWithProcess%d", (int)(getpid()));
}

///Funcion que lee un articulo de un archivo y los imprime (Otra función), se añadio para poder leer articulos de un archivo e imprimirlos
void readArticles(){
    struct NewsArticle *article = malloc(sizeof(struct NewsArticle));
    read(pipeS, article, sizeof(struct NewsArticle));
    printArticle(article);
}

//Funcion que lee articulos de un archivo (Otra función) permanentemente, se añadio para poder leer articulos de un archivo permanentemente
void readTrue()
{
    do
    {
        pipeS = mkfifo(pipename, fifo_mode);
        if (pipeS == -1)
        {
            perror("Error creando el pipe");
            unlink(pipename);
            printf("Se volvera a intentar\n");
            sleep(5);
        }
    } while (pipeS == -1);
    do
    {
        pipeS = open(pipename, O_RDONLY);
        if (pipeS == -1)
        {
            perror("Error abriendo el pipe");
            printf("Se volvera a intentar\n");
            sleep(5);
        }
    } while (pipeS == -1);
    while(true)
        readArticles();

}

//Funcion que envia una suscripcion al SC dada una serie de categorias, se añadio para poder enviar una suscripcion al SC
bool sendSubscription(char *categories)
{
    write(fd, createSubscribe(getpid(), pipename, categories), sizeof(struct Subscribe));
    return true;
}

//Funcion que lee una suscripcion del SC y la envia (Otra función), se añadio para poder leer una suscripcion y enviarla al SC
void writeSub()
{
    char *text = malloc(sizeof(char) * 90);
    printf("Introduzca las categorias a las que se planea suscribir (Maximo 80 caracteres) (Separadas por un guion bajo: _, si solo es una, no es necesario): ");
    scanf("%s", text);
    if (sendSubscription(text))
        printf("Suscrito correctamente\n");
    else
        printf("No se ha podido suscribir\n");
    free(text);
}

//Funcion que envia suscripciones permanentemente al SC, se añadio para poder enviar suscripciones permanentemente al SC
void writeTrue()
{
    while (true)
        writeSub();
}

//Funcion que finaliza el suscriptor, se añadio para poder finalizar el suscriptor
void end()
{
    unlink(pipename);
    exit(0);
}

//Funcion para capturar una señal y poder finalizar el suscriptor correctamente, se añadio para poder capturar una señal y finalizar el suscriptor
void catch_sigint()
{
    write(STDOUT_FILENO, "END", 4);
    end();
}

//Funcion para capturar una señal y poder finalizar el suscriptor correctamente, se añadio para poder capturar una señal y finalizar el suscriptor
void catch_sigterm()
{
    write(STDOUT_FILENO, "TERMINATE", 10);
    end();
}

int main(int argc, char **argv)
{
    startSystem(argc, argv);
    signal(SIGINT, catch_sigint);
    signal(SIGTERM, catch_sigterm);
    pthread_create(&thread_id1, NULL, (void *)(writeTrue), NULL);
    readTrue();
    return 0;
}
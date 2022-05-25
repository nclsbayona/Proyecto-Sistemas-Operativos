/*
file: subscriber.c
Authors: Nicolas Bayona, Manuel Rios, Abril Cano 
Contains: implementation of functions of the subscriber 
that connects to the cental system in a 
Publisher-Subscriber system
Date of last update: 24/05/2022
*/

#include "news.c"
#include "subscribe.c"
int pipeS, fd, close_status;
char *with_system;
char pipename[100];
pthread_t thread_id1;

//Funcion para imprimir un articulo, se añadio para imprimir un articulo
/**
 * Function: printArticle
 * Parameters:  article A pointer to a NewsArticle struct that represents the article to be printed
 * Returns: none
 * Description: The function takes a pointer to a NewsArticle struct as an argument, and prints the category and
 * text of the article
 */
void printArticle(struct NewsArticle *article)
{
    printf("\nNew Article:\n%c: %s\n\n", article->category, article->text);
}

//Funcion que inicializa lo relacionado con un suscriptor, se añadio para poder inicializar un suscriptor
/**
 * Function: startSystem
 * Parameters: argc The number of arguments passed to the program. argv The command line arguments
 * Returns: none
 * Description: initializes a subscriber. It opens a pipe to the system process, and then creates a pipe to the process that called it
 */
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
/**
 * Function: readArticles
 * Parameters: none
 * Returns: none
 * Description: It reads the article from the pipe and prints it
 */
void readArticles(){
    struct NewsArticle *article = malloc(sizeof(struct NewsArticle));
    read(pipeS, article, sizeof(struct NewsArticle));
    printArticle(article);
}

//Funcion que lee articulos de un archivo (Otra función) permanentemente, se añadio para poder leer articulos de un archivo permanentemente
/**
 * Function: readTrue
 * Parameters: none
 * Returns: none
 * Description: It creates a named pipe, opens it, and then reads from it. Reads the articles form file
 */
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
/**
 * Function: sendSubscription
 * Parameters: categories a string of categories to subscribe to, separated by commas.
 * Returns: boolean that determines the success of the subscription
 * Description: It sends a subscription to the central system
 */
bool sendSubscription(char *categories)
{
    write(fd, createSubscribe(getpid(), pipename, categories), sizeof(struct Subscribe));
    return true;
}

//Funcion que lee una suscripcion del SC y la envia (Otra función), se añadio para poder leer una suscripcion y enviarla al SC
/**
 * Function: writeSub
 * Parameters: none
 * Returns: none
 * Description: It asks the user the category to subscribe, then sends the subscription
 */
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
/**
 * Function: writeTrue
 * Parameters: none
 * Returns: none
 * Description: Permanently sends susbscriptions to the central system
 */
void writeTrue()
{
    while (true)
        writeSub();
}

//Funcion que finaliza el suscriptor, se añadio para poder finalizar el suscriptor
/*
  Function: end
  Paremeters: none
  Returns: none
  Description: It kills all the processes that are still alive
*/
void end()
{
    unlink(pipename);
    exit(0);
}

//Funcion para capturar una señal y poder finalizar el suscriptor correctamente, se añadio para poder capturar una señal y finalizar el suscriptor
/**
 * Function: catch_sigint
 * Paramenters: none
 * Returns: none
 * Description: init signal handler, captures a signal that allows the central system to end correctly
 */
void catch_sigint()
{
    write(STDOUT_FILENO, "END", 4);
    end();
}

//Funcion para capturar una señal y poder finalizar el suscriptor correctamente, se añadio para poder capturar una señal y finalizar el suscriptor
/**
 * Function: catch_sigint
 * Paramenters: none
 * Returns: none
 * Description: term signal handler, captures a signal that allows the central system to end correctly
 */
void catch_sigterm()
{
    write(STDOUT_FILENO, "TERMINATE", 10);
    end();
}

/**
 * Function: main
 * Parameters: argc The number of arguments passed to the program, argv the arguments necesary to run the program 
 * Returns: the return value of the function is the exit status of the program.
 * Description: It's a function that reads the arguments and starts the subscriber
 */
int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Sintaxis invalida, revise los argumentos ingresados\n");
    }

    if (strcmp("-s", argv[1]) != 0 ){
        printf("Sintaxis invalida, revise los argumentos ingresados\n");
        return -1;
    }

    startSystem(argc, argv);
    signal(SIGINT, catch_sigint);
    signal(SIGTERM, catch_sigterm);
    pthread_create(&thread_id1, NULL, (void *)(writeTrue), NULL);
    readTrue();
    return 0;
}
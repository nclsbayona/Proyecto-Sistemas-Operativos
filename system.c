#include "coms.c"
#include <string.h>

char* with_publishers;
char* with_subscriptors;
int time;

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

int main(int argc, char **argv)
{
    startSystem(argc, argv);
    printf(" -p %s\n", with_publishers);
    printf(" -s %s\n", with_subscriptors);
    printf(" -t %d\n", time);
    return 0;
}
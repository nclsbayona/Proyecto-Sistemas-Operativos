#include "coms.c"

char *with_system;

void startSystem(int argc, char **argv)
{
    for (int i = 1; i < argc; i += 2)
        if (strcmp(argv[i], "-s") == 0)
            with_system = argv[i + 1];     
}

int main(int argc, char** argv){

    return 0;
}
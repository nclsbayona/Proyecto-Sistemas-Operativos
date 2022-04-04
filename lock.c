//Compartiendo una variable entre procesos
#include <stdbool.h>

bool lock=false;

void lockSend(){
    lock=true;
}

void unlockSend(){
    lock=false;
}
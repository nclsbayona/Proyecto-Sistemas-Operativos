//Compartiendo una variable entre procesos
#include <stdbool.h>
#include <sys/mman.h>
#include <stdlib.h>
int *lock;

int getLock(){
    return *lock;
}

void lockSend(){
    *lock=1;
}

void unlockSend(){
    *lock=0;
}

void endLock(){
    munmap(lock, sizeof(int));
    exit(0);
}

void start(){
lock=(int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
unlockSend();
}
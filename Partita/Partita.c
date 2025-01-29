#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../Librerie/Stanze/Stanze.h"


int main(int argc, char *argv[]){
    Stanza s;
    read(atoi(argv[1]), &s, sizeof(Stanza));
    printf("porco dio: %d\n", s.stato);
    return 0;
}
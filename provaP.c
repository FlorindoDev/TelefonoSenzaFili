#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


int main() {
    pid_t pid = fork();

    if(pid == 0){
        printf("io sono figlio\n");
        //execl("culos", "culos", NULL);
        execl("/bin/ls", "ls", "-l", "-a", NULL); //lista
    }else{
        printf("io sono padre\n");
    }
    printf("io sono padre di negor\n");
}

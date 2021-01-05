#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
//Client creates private FIFO
//Client writes private FIFO name to server
//Server responds

int wkp, fifo;

static void sighandler(int signo)
{
    if (signo == SIGINT)
    {
        write(wkp, "STOP", 5);
        exit(0);
    }
}

int main(){
    signal(SIGINT, sighandler);

    //Client creates private FIFO
    char fifo_name[256];
    sprintf(fifo_name, "%d", getpid());
    mkfifo(fifo_name, 0644);
    printf("[Client] Private FIFO created\n");

    //Client writes private FIFO name to server
    wkp = open("wkp", O_WRONLY);
    printf("[Client] Connected to well-known-pipe\n");
    write(wkp, fifo_name, sizeof(fifo_name));
    printf("[Client] Wrote the private FIFO name to the well-known-pipe\n");

    //Process server response
    char response[256];
    fifo = open(fifo_name, O_RDONLY);
    read(fifo, response, 256);
    printf("Server response recieved: %s\n", response);
    remove(fifo_name);

    //Send acknowledgement back
    char acknowledgement[256] = "Client OK";
    write(wkp, acknowledgement, sizeof(fifo_name));
    printf("Wrote client acknowledgement: %s\n", acknowledgement);

    //From the previous assignment:
    int write_to = wkp;
    int read_from = fifo;
    printf("---- Free oracle consultation service ----\n");
    while(1){
        char buffer[500];
        printf("Ask a yes or no question: ");
        fgets(buffer, 500, stdin);
        int i = 0; 
        while(buffer[i]){
            if(buffer[i] == '\n') buffer[i] = '\0'; 
            i++;
        }
        write(write_to, buffer, 500);
        
        char readBuffer[45];
        read(read_from, readBuffer, 45);
        if(strcmp(readBuffer, "STOP") == 0) exit(0);
        printf("\n%s\n\n", readBuffer);

    }
}
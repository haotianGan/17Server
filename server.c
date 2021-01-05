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

int wkp_fd;
int private_fd;

static void sighandler(int signo)
{
    if (signo == SIGINT)
    {
        write(private_fd, "STOP", 5);
        exit(0);
    }
}

void handshake(){
    
    //Server creates WKP
    mkfifo("wkp", 0644);
    printf("[Server] WKP online\n");

    //Server reads from WKP for private pipe
    wkp_fd = open("wkp", O_RDONLY);
    char privatePipe[256];
    read(wkp_fd, privatePipe, 256);
    printf("[Server] Recieved private pipe from WKP: #%s\n", privatePipe);
    remove("wkp"); //Server gets rid of public pipe

    //Server writes private ack to private pipe
    private_fd = open(privatePipe, O_WRONLY);
    write(private_fd, "Server OK", 256);
    printf("[Server] Sent 'SERVER OK' to client\n");

    //Server reads for client ack
    char clientAck[256];
    read(wkp_fd, clientAck, 256);
    printf("[Server] Client got my message. Client response: %s\n", clientAck);
}

int main(){
    signal(SIGINT, sighandler);
    while(1){
    handshake();
    //From the previous assignment:
    int read_from = wkp_fd;
    int write_to = private_fd;
        while(1){
            char buffer[500];
            read(read_from, buffer, 500);
            printf("Query recieved from client: %s\n", buffer);
            if(strcmp(buffer, "STOP") == 0) break;
            int c = 0;
            int i = 0;
            while(buffer[i]) {
                if(buffer[i] == ' ') c++;
                i++;
            }
            if(c % 2 == 0){
                write(write_to, "Oracle: I have determined the answer is yes.", 45);
            } else {
                write(write_to, "Oracle: I have determined the answer is no.", 45); 
            }
        }
    }
}
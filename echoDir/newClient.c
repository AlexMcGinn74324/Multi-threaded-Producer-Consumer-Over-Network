#include <stdio.h>
#include <string.h>	//strlen
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>	//inet_addr

#define BUF_SIZE 2000

int main(int argc , char *argv[]){
    char server_reply[2000];
    int port = 1234;
    if(argc >= 2){
        port = atoi(argv[1]);
    }


    int socket_desc = 0;
    char buf[BUF_SIZE];
    struct sockaddr_in server;
    memset(buf, '0',sizeof(buf));



    //Create socket
    if( (socket_desc = socket(AF_INET , SOCK_STREAM , 0)) < 0 ){
        perror("Socket");
    }

    memset(&server, '0', sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

//    char* ipAddr = "129.32.95.111\0";
    if(inet_pton(AF_INET, argv[2], &server.sin_addr)<=0){
        printf("\n inet_pton error occured\n");
        return 1;
    }

    printf("%d\n", port);
//    char *message;



    puts("Hangs here:");
    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Connect");
        return 1;
    }
    puts("Never here");

    puts("Client connection complete\n");

    //get input from keyboard and write to socket, read it back and write to stdout
    int numRead;
    while (fgets(buf, BUF_SIZE, stdin) != NULL) {
        write(socket_desc, buf, strlen(buf));
        numRead = read(socket_desc, buf, BUF_SIZE);
        write(1, buf, numRead);
    }
    write(socket_desc, "-1\n", 2);


    puts("EOF received");

//    Receive a reply from the server
    if( recv(socket_desc, server_reply , 2000 , 0) < 0){
        puts("recv failed");
    }else{
        puts("Reply received\n");
    }
    puts(server_reply);

    return 0;
}
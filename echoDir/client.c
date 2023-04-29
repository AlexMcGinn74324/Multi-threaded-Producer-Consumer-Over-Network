#include <stdio.h>
#include <string.h>	//strlen
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>	//inet_addr

#define BUF_SIZE 2000

int main(int argc , char *argv[]){
    int port = 1234;

    if(argc == 2){
        port = atoi(argv[1]);
    }

    printf("%d\n", port);
    int socket_desc;
    struct sockaddr_in server;
//    char *message;
    char buf[BUF_SIZE];
    char server_reply[2000];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }

    //converts host add from IPv4 numbers and dot notation into binary data in network byte order
    server.sin_addr.s_addr = inet_addr("10.2.97.108");


//    if(inet_pton(AF_INET,"127.0.0.1", &server.sin_addr.s_addr)<=0){
//        printf("\n inet_pton error occured\n");
//        return 1;
//    }

    //Set family as TCP
    server.sin_family = AF_INET;

    //htons converts from host byte order to network byte order
    server.sin_port = htons(port);

    puts("Hangs here:");
    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Connect");
        return 1;
    }
    puts("Never here");

    puts("Client connection complete\n");

    //Send one message
//    message = "Stuff and things!\n\0";
//    if( send(socket_desc , message , strlen(message) , 0) < 0){
//        puts("Send failed");
//        return 1;
//    }

    //take continuous
//    ssize_t numRead;
//    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
//        if (send(socket_desc, buf, numRead, 0) != numRead)

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
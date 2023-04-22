#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>	//inet_addr

#define BUF_SIZE 2000
void *connection_handler(void *socket_desc);
int main(int argc , char *argv[]){
        if(argc <= 1){
        puts("Please input port");
        exit(1);
    }
    int port = atoi(argv[1]);
    int socket_desc , new_socket , c;
    struct sockaddr_in server , client;
    char outMessage[2000] = "I have received your message";
    char inMessage[BUF_SIZE];
    ssize_t numRead;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Bind failed");
        exit(1);
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 10);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (new_socket<0){
        perror("accept failed");
    }

    puts("Connection accepted");

//    connection_handler((void*)(&new_socket));

    write(new_socket, &outMessage,2000);

    while ((numRead = read(new_socket, inMessage, BUF_SIZE)) > 0)
        if (write(STDOUT_FILENO, inMessage, numRead) != numRead)
            perror("write");
//    inMessage[count] = '\0';
//    printf("%s\n", inMessage);


    return 0;
}
void *connection_handler(void *socket_desc)
{

    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    printf("Socket descriptor: %d\n", sock);
    int read_size;
    char *message , client_message[2000];

    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));

    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , client_message , strlen(client_message));
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}
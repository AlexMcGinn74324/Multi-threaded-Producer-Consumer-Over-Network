#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "helpers.h"
//====================================Constants
#define BUF_SIZE 2000
//==================================
/* This process will run the producers in our lab. They create 'products' in a loop over 150 iterations.
 * Each iteration they send 1 product into the socket and briefly delay (randomly between .01-.2s). The data
 * to be sent over is held inside a 'data' struct, which holds the product type, consumption thread id, product
 * count, and consumption count. */
//input: (from commandline) Port #, Product type #
//output: Writes the current product type/count to socket shared with distributor threads
//====================================================================================================
int main(int argc, char* argv[]){
    int random = 0, max = 20, min = 1; //~~~~~~~~~~REINITIALIZE THIS!!!!!!!!~~~~~~~~~
    srand(time(NULL));
    int port = 1234, pType = -1, pCount = 0; //product type and current count
//    size_t size = 16;   //size of data struct
    //socket variables
    int socket_desc;
    struct sockaddr_in server;
//    char buf[BUF_SIZE];
//    char server_reply[BUF_SIZE];

    if(argc > 2){
        port = atoi(argv[1]);
        pType = atoi(argv[2]);
    }    //convert port number to int


    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }

    //Set up address struct
    //converts host add from IPv4 numbers and dots notation into binary data in network byte order
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Set family as TCP
    server.sin_family = AF_INET;

    //htons converts from host byte order to network byte order
    server.sin_port = htons(port);

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Connect");
        return 1;
    }

//    puts("Client connection complete\n");

//    Test: Send one message
//    char* message = "Stuff and things!\n\0";
//    if( send(socket_desc , message , strlen(message) , 0) < 0){
//        puts("Send failed");
//        return 1;
//    }


    //loop to create product
    for(int i = 0; i <= 150; i++){
        pCount++;
        data cur = {pType,pCount, 0, 0}; //initialize each element, consumer count/thread id to be updated later

//        Generate random numbers between .01-.2s
        random = (rand() % (max + 1 - min)) + min;
        unsigned int timer = random * 10000;
//        printf("Rand = %d\n", timer);

        if( (usleep(timer)) == -1) { //sleep
            perror("usleep in producer function");
            exit(1);
        }

//        printf("%d\n", cur.pCount);

        if(i == 150){   //the 151st iteration
            cur.pCount = -1;
        }
        int len = sizeof(cur);
        if( send(socket_desc , &cur , len , 0) < 0){
            puts("Send failed");
            return 1;
        }
    }

}

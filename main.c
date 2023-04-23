#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "helpers.h"
//================================Left off
/* Was able to connect to 1 client and accept a single message
 *
 * Other:
 * Probably shouldn't be closing client/server fd during/after while loop
 */
//=============================Constants
#define MAX1 60
#define MAX2 75
#define BUF_SIZE 4096
//===============buffers
struct Queue* q1;
struct Queue* q2;
//==============Globals
pthread_cond_t filled1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t filled2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
struct locks lock1;
struct locks lock2;
struct consumerBundle* cb1;
struct consumerBundle* cb2;
pthread_mutex_t fMutex = PTHREAD_MUTEX_INITIALIZER;
//==============
void put(int value, int buffer[], int fill_ptr, int count, int max);
int get(int use_ptr, int buffer[], int count, int max);
void distributor(int* fd);

/* Before getting into main we've initialized all of our locks and condition variables
 * that will be used to provide mutual exclusion to our buffers (provided via a queue
 * that holds our product type, product count, size, max size, thread id, and consumption
 * count. We fork our producers that create and write these product type/count to a mutual
 * pipe that it shares with the distributor function. We then redirect output to a file,
 * create our threads for each consumption process, which asynchronously takes items from
 * the distributor and writes them to file. The main thread then runs the distributor
 * thread, which again takes items from the pipe and adds them to our buffer (asynchr
 * -onously), before returning to join/reap child threads/processes.  */
int main(int argc, char* argv[]){
    q1 = createQueue(MAX1);   //creates queue, assigns front and rear to NULL
    q2 = createQueue(MAX2);
    pthread_t c1, c2, c3, c4;
    pthread_t d1, d2;   //distributor threads

    //=====================lock structs set
    lock1.filled = &filled1;
    lock1.empty = &empty1;
    lock1.mutex = &mutex1;
    lock2.filled = &filled2;
    lock2.empty = &empty2;
    lock2.mutex = &mutex2;

    cb1 = (struct consumerBundle*)malloc(sizeof(struct consumerBundle));
    cb1->q = q1;
    cb1->lock = &lock1;
    cb1->flag = 0;  //we haven't received kill signal
    cb1->cNum = 1;
    cb1->fMutex = &fMutex;  //lock for file access

    cb2 = (struct consumerBundle*)malloc(sizeof(struct consumerBundle));
    cb2->q = q2;
    cb2->lock = &lock2;
    cb2->flag = 0;  //we haven't received kill signal
    cb2->cNum = 1;
    cb2->fMutex = &fMutex;
    //======================Socket Code
    int serverFd, clientFd;
    struct sockaddr_in server, client;
    int port = 1234, connections = 0;    //default port, connection counter
    unsigned int len;
    char buf[1024];

    //make sure the port is entered w/ correct # args
    if(argc >= 2){
        port = atoi(argv[1]);
    }else{
        port = 8888;
    }

    //assign socket with TCP
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        perror("Cannot create socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    len = sizeof(server);

    //bind to our server
    if (bind(serverFd, (struct sockaddr *)&server, len) < 0) {
        perror("Cannot bind sokcet");
        exit(2);
    }
    //start listening for up to 10 connections at a time
    if (listen(serverFd, 10) < 0) {
        perror("Listen error");
        exit(3);
    }

    while (connections < 2) {   //keep running until we have 2 connections
        len = sizeof(client);
        printf("waiting for clients\n");

        if ((clientFd = accept(serverFd, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept error");
            exit(1);
        }else{
            connections++;
        }

        //converts client address from network byte order to string IPv4 dotted decimal
        //returned in a statically allocated buffer, which subsequent calls will overwrite
        char *client_ip = inet_ntoa(client.sin_addr);

        //prints client ip and port
        printf("Accepted new connection from a client %s:%d\n\n", client_ip, ntohs(client.sin_port));

        pthread_create(&d1, NULL, distributor, ((void*)clientFd));



        memset(buf, 0, sizeof(buf));    //set size/initialize to 0
//        char buf[BUF_SIZE];
        ssize_t numRead;
        data data = {0,0, 0, 0};
        ssize_t dLen = sizeof(data);
        while ((numRead = read(clientFd, &data, dLen)) > 0) {
//            write(1, buf, numRead); //write to stdout
            //write to client what we read
//            if (write(clientFd, buf, numRead) != numRead) {
//                perror("write");
//                exit(EXIT_FAILURE);
//            }
//            printf("Count: %d\n", data.pCount);
        }
            printf("Last count: %d\n", data.pCount);

//        printf("%zu\n", write(clientFd, "Service Complete", 100));
        close(clientFd);
    }
    close(serverFd);
    //run two distributor threads here



    //start consumer threads withoutput redirected
//    int out = open("out.txt", O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO );
//    if( (dup2(out, STDOUT_FILENO)) == -1){
//        perror("dup2 in main");
//        exit(1);
//    }
//
//    //create consumer threads
//    pthread_create(&c1, NULL, consumer, ((void*)cb1));
//    pthread_create(&c2, NULL, consumer, ((void*)cb1));
//    pthread_create(&c3, NULL, consumer, ((void*)cb2));
//    pthread_create(&c4, NULL, consumer, ((void*)cb2));

//    if( (pthread_join(c1, NULL) == 0)){
//        printf("Thread 1 join successful\n");
//    }
//    if( (pthread_join(c2, NULL) == 0)){
//        printf("Thread 2 join successful\n");
//    }
//    if( (pthread_join(c3, NULL) == 0)){
//        printf("Thread 3 join successful\n");
//    }
//    if( (pthread_join(c4, NULL) == 0)){
//        printf("Thread 4 join successful\n");
//    }

    if(c1 == c3 && c2 == c4);

    return 0;
}
//==============================================================Distributor Thread
//Input: fds for pipe shared with producer processes
//Output: Adds items to buffers (Queues)
/* The distributor is responsible for reading from the
 * pipe that it shares with the producer processes. It
 * then takes that data and adds it to the buffer that
 * the consumer threads then write to file.
 */
void distributor(void* fd){
    int done = 0;
    data new = {0, 0, 0, 0};

    int sd


    //continue to read until both sentinel values are sent
    while(done < 2){
        //read one record each time
        if( (read(fd[0],&new, 16)) == -1){
            perror("read in distributor");
            exit(1);
        }

        if(new.pCount == -1){   //if a thread is finished let the producer know
            done++;
        }

        if(new.pType == 1){
            enQueue(q1, new.pType, new.pCount, &lock1);
        }else if(new.pType == 2){
            enQueue(q2, new.pType, new.pCount, &lock2);
        }
        if(done == 2){
            break;
        }
    }
    return;
}
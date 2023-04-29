#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include "helpers.h"

//==========================================================CONSUMER===================================================
/* Input: ConsumerBundle struct (void *)
 * Output: writes retrieved node from buffer into file (using redirection from main)
 * Writes the product type, count, current size of buffer, consumption count, and the thread ID that is writing it

 * The Consumer thread process uses locks and condition variables to
 * provide asynchronous operations on the shared buffer and file. We
 * have access to these values through the consumer bundle struct. This
 * allows us to sleep while the buffer is empty and wake the distributor
 * once we have entered values into it. There was a problem with notifying
 * the second thread that the first had finished, which we solved using a
 * flag inside the consumer bundle struct.
 */
void* consumer(void* cbTemp){
    struct consumerBundle *cb = (struct consumerBundle *) cbTemp;   //transferring address isn't a race condition
    struct QNode *node = newNode(0, 0);


    while(cb->flag != 1) {  //===================Loop until flag is thrown

        if (pthread_mutex_lock((cb->lock->mutex)) != 0) {   //===============LOCKED
            perror("consumer lock 1");
            exit(1);
        }
        if(cb->flag == 1){
            printf("Flag received 1 by %zu\n", pthread_self());
            return NULL;
        }

        while(cb->q->size == 0 && cb->flag != 1) {
            if ((pthread_cond_wait((cb->lock->empty), (cb->lock->mutex))) != 0) {
                perror("Cond wait in consumer 1");
                exit(1);
            }
            if(cb->flag == 1){
                return NULL;
            }
        }
        
        //queue size decremented in deQueue
        if(cb->flag != 1)
            node = deQueue(cb->q);

        if(node->pCount != -1){
                pthread_mutex_lock(cb->fMutex);
                printf("%34s %3s %3s %16s %13s\n", "Type", "P.Count", "Size", "Consumption Count", "C. Thread ID");
                printf("Test Node Returned in Consumer: %1d %6d %3d %14d %25zu\n", node->pType, node->pCount, cb->q->size, cb->cNum,pthread_self());
                pthread_mutex_unlock(cb->fMutex);
        }else{  //we have received kill signal for this product
            cb->flag = 1;
        }

        if(cb->flag != 1)
            cb->cNum++;

        if ((pthread_cond_signal(cb->lock->filled)) != 0) {
            perror("cond signal in consumer");
            exit(1);
        }
        pthread_mutex_unlock((cb->lock->mutex));
        //=========================================UNLOCKED, loop again
    }
//if the first thread for this buffer has terminated successfully, we signal the other one to stop waiting
    if( (pthread_cond_signal(cb->lock->empty)) != 0){
        perror("Final signal in consumer");
        exit(1);
    }
    return NULL;
}

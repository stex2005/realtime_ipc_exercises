#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PRODUCER_LOOPS 0x000EFFFF
#define COSTUMER_LOOPS 0X000EFFFF
#define STORAGE_DIMENSION 10

pthread_t tid[2];
int counter = 0;
int lock_counter = 0;
pthread_mutex_t lock;
pthread_cond_t notempty, notfull;

void* producer(void* arg)
{
    while(1){

        // Trylock returns 0 if the lock was succesfull!
//        if(pthread_mutex_trylock(&lock)){
//            lock_counter++;
//            printf("Lock counter: %d\n",lock_counter);
//        }

        // Lock mutex
//        pthread_mutex_lock(&lock);


        // If storage is full
//        if(counter >= STORAGE_DIMENSION){
//            // Raise condition
//            printf("Wait for costumer");
//            pthread_cond_wait(&notfull,&lock);
//        }

        // Once released, fill storage
        counter += 1;

        printf("\n Job +1: %d\n", counter);

        // Release condition for empty storage
//        pthread_cond_signal(&notempty);

        // Unlock mutex
//        pthread_mutex_unlock(&lock);

        // Do work here
        for (unsigned long i = 0; i < PRODUCER_LOOPS; i++);



    }
    return NULL;
}

void* costumer(void* arg)
{
    while(1){

        // Trylock returns 0 if the lock was succesfull!
//        if(pthread_mutex_trylock(&lock)){
//            lock_counter++;
//            printf("Lock counter: %d\n",lock_counter);
//        }

        // Lock mutex
//        pthread_mutex_lock(&lock);

        // If storage is empty
//        if(counter == 0){
//            // Wait for non-empty condition
//            printf("Wait for producer");
//            pthread_cond_wait(&notempty,&lock);
//        }

        // Empty storage
        counter -= 1;

        printf("\n Job -1: %d\n", counter);

        // Release condition for full storage
//        pthread_cond_signal(&notfull);

        // Unlock mutex
//        pthread_mutex_unlock(&lock);

        // Do work here
        for (unsigned long i = 0; i < COSTUMER_LOOPS; i++);




    }
    return NULL;
}

int main(void)
{
    int i = 0;
    int error;

    // Initialize mutex
//    pthread_mutex_init(&lock, NULL);


    // Initialize condition variables
//    pthread_cond_init(&notempty, NULL);
//    pthread_cond_init(&notfull, NULL);

    // Create two threads
    while (i < 2) {
        if(i==0)
            error = pthread_create(&(tid[i]),
                                   NULL,
                                   &producer, NULL);
        else
            error = pthread_create(&(tid[i]),
                                   NULL,
                                   &costumer, NULL);
        printf("\n Created thread %d", i+1);
        if (error != 0)
            printf("\nThread can't be created :[%s]",
                   strerror(error));
        i++;
    }

    // Wait for threads to end!
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    // Destroy mutex
//    pthread_mutex_destroy(&lock);

    return 0;
}

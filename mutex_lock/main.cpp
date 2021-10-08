#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_t tid[2];
int counter = 0;
pthread_mutex_t lock;
pthread_cond_t notempty, notfull;

void* trythis(void* arg)
{
    while(1){
    pthread_mutex_lock(&lock);

    if(counter == 1)
        pthread_cond_wait(&notfull,&lock);
    counter += 1;
    printf("\n Job +1: %d\n", counter);
    pthread_cond_signal(&notempty);

    pthread_mutex_unlock(&lock);


    //    for (i = 0; i < (0xFFFFFFFF); i++)
    for (unsigned long i = 0; i <   (0x0FFFFFFF); i++) ;

//    printf("\n Job %d has finished\n", counter);


     }
    return NULL;
}

void* trythis2(void* arg)
{
    while(1){

    pthread_mutex_lock(&lock);

    if(counter == 0)
        pthread_cond_wait(&notempty,&lock);
    counter -= 1;
    printf("\n Job -1: %d\n", counter);
    pthread_cond_signal(&notfull);

    pthread_mutex_unlock(&lock);


    //    for (i = 0; i < (0xFFFFFFFF); i++)
//    for (unsigned long i = 0; i <   (0x00FFFFFF); i++)

//        ;

//    printf("\n Job %d has finished\n", counter);

    }
    return NULL;
}

int main(void)
{
    int i = 0;
    int error;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    pthread_cond_init(&notempty, NULL);
    pthread_cond_init(&notfull, NULL);

    while (i < 2) {
        if(i==0)
            error = pthread_create(&(tid[i]),
                                   NULL,
                                   &trythis, NULL);
        else
            error = pthread_create(&(tid[i]),
                                   NULL,
                                   &trythis2, NULL);
        printf("\n Created thread %d", i+1);
        if (error != 0)
            printf("\nThread can't be created :[%s]",
                   strerror(error));
        i++;
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_mutex_destroy(&lock);

    return 0;
}

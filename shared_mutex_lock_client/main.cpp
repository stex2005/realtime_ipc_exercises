#include <iostream>
#include <string.h> // For strerror

#include <fcntl.h>
#include "semaphore.h"

#include "shared_packet.h"

#define ITERATIONS 0xFFF
#define LOOPS 0x0FFFFFFF


using namespace std;

shared_packet shm;
pthread_mutexattr_t mutexattr;
pthread_condattr_t condattr;

int counter = 0;
int counter_wait = 0;

void* client_routine(void* arg){

    while(counter<ITERATIONS){

        // Lock semaphores
        pthread_mutex_lock(&shm.data->lock);
        cout << "\n Client adds: " << shm.data->counter << " Counter: " << counter <<endl;

        // If server has not acted yet
        if(shm.data->counter == 0){
            // Wait until client is ready
            cout << " Wait for server " << endl;
//            pthread_cond_wait(&shm.data->server_ready,&shm.data->lock);
            counter_wait++;
        }
        // Server acts
        shm.data->counter -=1;
        // Remove block
//        pthread_cond_signal(&shm.data->client_ready);

        pthread_mutex_unlock(&shm.data->lock);


        // Do stuff
        for (unsigned long i = 0; i <   (LOOPS); i++) ;
        counter++;

    }

}


int main()
{
    cout << "Hello World! Shared mutex over shared memory client" << endl;

    if (shm.init())
    {
        int key = shm.get_shared_memory_key();
        printf("shared memory initialized with key 0x%x", key);    // start the shared memory communication
    }
    else
    {
        cout << "shared memory initialization has been failed";
        shm.detach_shared_memory();
    }

    // Create server thread
    pthread_t tid;
    int error = 0;
    error = pthread_create(&(tid),
                           NULL,
                           &client_routine, NULL);

    printf("\n Created thread with id: %lu", tid);
    if (error != 0)
        printf("\nThread can't be created :[%s]",
               strerror(error));

    pthread_join(tid,NULL);

    cout << "Counter for locking actions: " << counter_wait << endl;
    printf("\n Shutting down.. \n");
    shm.detach_shared_memory();
//    pthread_mutex_destroy(&shm.data->lock);
//    pthread_cond_destroy(&shm.data->cond);
//    pthread_cond_destroy(&shm.data->client_ready);
    return 0;
}

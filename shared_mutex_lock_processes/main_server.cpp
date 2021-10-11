#include <iostream>
#include <string.h> // For strerror

#include <fcntl.h>
#include "semaphore.h"

#include "shared_packet.h"

#define ITERATIONS 1000
#define LOOPS 0x0FFFFFFF


using namespace std;

shared_packet shm;
pthread_mutexattr_t mutexattr;
pthread_condattr_t condattr;

int counter = 0;
int counter_wait = 0;

void* server_routine(void* arg){
    cout << "Server routine started." <<endl;
    while(counter<ITERATIONS){

        // Lock semaphores
        pthread_mutex_lock(&shm.data->lock);
        cout << "\n Server job: " << shm.data->counter << " Counter: " << counter <<endl;
        // If client has not acted yet
//        if(shm.data->counter >= 10){
//            // Wait until client is ready
//            cout << "Wait for client" << endl;
//            pthread_cond_wait(&shm.data->client_ready,&shm.data->lock);
//            counter_wait++;
//        }
        // Server acts
        shm.data->counter += 1;
        // Remove block
//        if(shm.data->counter > 0)
//            pthread_cond_signal(&shm.data->server_ready);

        pthread_mutex_unlock(&shm.data->lock);

        // Do stuff
        for (unsigned long i = 0; i <   (LOOPS); i++) ;
        counter++;
    }
}

int main()
{
    cout << "Hello World! Shared mutex over shared memory server" << endl;

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

    // Mutex initialization
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shm.data->lock, &mutexattr);
    pthread_mutexattr_destroy(&mutexattr);


    // Condition variables initialization
    pthread_condattr_init(&condattr);
    pthread_condattr_setpshared(&condattr,PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&shm.data->server_ready, &condattr);
    pthread_cond_init(&shm.data->client_ready, &condattr);
    pthread_condattr_destroy(&condattr);

    // Create server thread
    pthread_t tid;
    int error = 0;
    error = pthread_create(&(tid),
                           NULL,
                           &server_routine, NULL);

    printf("\n Created thread with id: %lu \n", tid);
    if (error != 0)
        printf("\nThread can't be created :[%s]",
               strerror(error));

    pthread_join(tid,NULL);

    cout << "Counter for locking actions: " << counter_wait << endl;
    printf("\n Shutting down.. \n");
    pthread_mutex_destroy(&shm.data->lock);
    pthread_cond_destroy(&shm.data->server_ready);
    pthread_cond_destroy(&shm.data->client_ready);

    shm.detach_shared_memory();

    return 0;
}

#include <iostream>

#include <fcntl.h>
#include "semaphore.h"

#include "shared_packet.h"

using namespace std;

int main()
{
    cout << "Hello World! Shared mutex server" << endl;
    sem_t *mutex = sem_open("locker", O_CREAT,0600,1);

    shared_packet shm;
    shm.init();

    int counter = 0;

//    sem_close(mutex);
    sem_post(mutex);

    while(1){

        // Lock semaphores
        sem_wait(mutex);

        counter += 1;
        shm.data->counter += 1;

        printf("\n Job +1: %d\n", shm.data->counter);

        for (unsigned long i = 0; i <   (0x000FFFFF); i++) ;

        sem_post(mutex);

    }
    sem_close(mutex);
    return 0;
}

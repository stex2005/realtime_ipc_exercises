#ifndef SHARED_PACKET_H
#define SHARED_PACKET_H
#define DEFAULT_KEY_ID 202020

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <pthread.h>

class shared_packet
{
    struct shared_memory_packet {
        pthread_mutex_t lock;
        pthread_cond_t  server_ready;
        pthread_cond_t  client_ready;
        int counter;
        bool flag;
    };

private:
    key_t key;
    int shmid = 0;
public:

    shared_memory_packet* data;

    shared_packet();
    ~shared_packet();

    bool init();
    void change_shared_memory_key(key_t k); // only use this function before init
    key_t get_shared_memory_key(){return key;}
    void detach_shared_memory();
};

#endif // SHARED_PACKET_H

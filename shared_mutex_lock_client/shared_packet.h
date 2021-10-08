#ifndef SHARED_PACKET_H
#define SHARED_PACKET_H
#define DEFAULT_KEY_ID 6060

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

class shared_packet
{
    struct shared_memory_packet {
        int counter;
    };

private:
    key_t key;
    int shmid = 0;
public:

    shared_memory_packet* data;

    shared_packet();
    ~shared_packet();

    void init();
    void change_shared_memory_key(key_t k); // only use this function before init
    void detach_shared_memory();
};

#endif // SHARED_PACKET_H

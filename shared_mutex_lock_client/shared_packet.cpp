#include "shared_packet.h"

shared_packet::shared_packet()
{
        key = ftok("shmfile",DEFAULT_KEY_ID);
}

shared_packet::~shared_packet(){
    detach_shared_memory();
}

/**
 * @brief shared_packet::init
 */
void shared_packet::init(){
    // shmget returns an identifier in shmid
    shared_memory_packet temp;
    shmid = shmget(key, sizeof(temp),0666|IPC_CREAT);
    // shmat to attach to shared memory
    data = (shared_memory_packet*) shmat(shmid,(void*)0,0);
}

/**
 * @brief shared_packet::detach_shared_memory
 */
void shared_packet::detach_shared_memory(){
    shmdt(data);
    shmctl(shmid,IPC_RMID,NULL);
}

/**
 * @brief shared_packet::change_shared_memory_key
 * @param k
 */
void shared_packet::change_shared_memory_key(key_t k){
    key= k;
}

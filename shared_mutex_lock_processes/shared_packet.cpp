#include "shared_packet.h"
#include <iostream>

using namespace std;

shared_packet::shared_packet()
{
        key = ftok("/bin",DEFAULT_KEY_ID);
}

shared_packet::~shared_packet(){
    detach_shared_memory();
}

/**
 * @brief shared_packet::init
 */
bool shared_packet::init(){
    // shmget returns an identifier in shmid
    shared_memory_packet temp;
    shmid = shmget(key, sizeof(temp),0666|IPC_CREAT);
    // shmat to attach to shared memory
    data = (shared_memory_packet*) shmat(shmid,(void*)0,0);
    cout << shmid << " " << key <<endl;
    if (shmid > 0 && key > 0) return true; // no error
    else return false; // error
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

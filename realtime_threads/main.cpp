#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>               // cyclic loop
#include <pthread.h>            // multithread
#include <algorithm>            // std::copy
#include <signal.h>             // to catch ctrl-c
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <errno.h>
#include <limits.h>
#include <linux/unistd.h>
#include <fcntl.h>

#ifdef _POSIX_PRIORITY_SCHEDULING
#define POSIX "POSIX 1003.1b\n";
#endif
#ifdef _POSIX_THREADS
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
#define POSIX "POSIX 1003.1c\n";
#endif
#endif

#define DEFAULT_APP_DURATION_COUNTS 10000
#define DEFAULT_LOOP_TIME_NS        1000000L

using namespace std;

int loop_count = 0;

/***************************/
/*   GET SCHED PARAMETERS  */
/***************************/

static void getinfo ()
{
    struct sched_param param;
    int policy;

    sched_getparam(0, &param);
    printf("Priority of the process: %d\n\r", param.sched_priority);

    pthread_getschedparam(pthread_self(), &policy, &param);
    printf("Priority of the thread: %d, current policy is: %d\n\r",
              param.sched_priority, policy);
}

/********************/
/*   LATENCY TRICK  */
/********************/

static void set_latency_target(void)
{
    struct stat s;
    int err;
    int latency_target_value = 0;

    errno = 0;
    err = stat("/dev/cpu_dma_latency", &s);
    if (err == -1) {
        printf("WARN: stat /dev/cpu_dma_latency failed");
        return;
    }

    errno = 0;
    int latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    if (latency_target_fd == -1) {
        printf("WARN: open /dev/cpu_dma_latency");
        return;
    }

    errno = 0;
    err = write(latency_target_fd, &latency_target_value, 4);
    if (err < 1) {
        printf("# error setting cpu_dma_latency to %d!", latency_target_value);
        close(latency_target_fd);
        return;
    }
    printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);
}

/************************/
/*   CYCLIC THREAD      */
/************************/

void *thread(void * arg){

    getinfo();

    // Declare timespec variables
    struct timespec t_start, t_now, t_next, t_period, t_prev,t_result;

    // Get start and current time

    // Initialize next time to current time


    // While loop
    while(loop_count<DEFAULT_APP_DURATION_COUNTS) //ms
    {
        // Compute next cycle deadline (t_next)

        // Do stuff
        for(int i=0;i<1000;i++);
        loop_count++;

        // Sleep until the next cycle deadline (until t_next)

        // Get time after sleep (t_now)

        // Compute overflow (t_overflow = t_now - t_next)

        // If overflow > threshold, raise exception

    }

    cout << "Shutting down" << endl;
}

/************************/
/*         MAIN         */
/************************/

int main()
{
    printf("Max priority for SCHED_OTHER %d\n", sched_get_priority_max(SCHED_OTHER));
    printf("Min priority for SCHED_OTHER %d\n", sched_get_priority_min(SCHED_OTHER));
    printf("Max priority for SCHED_FIFO %d\n", sched_get_priority_max(SCHED_FIFO));
    printf("Min priority for SCHED_FIFO %d\n", sched_get_priority_min(SCHED_FIFO));
    printf("Max priority for SCHED_RR %d\n", sched_get_priority_max(SCHED_RR));
    printf("Min priority for SCHED_RR %d\n", sched_get_priority_min(SCHED_RR));

    // Scheduler variables
    int policy = 0;
    struct sched_param prio,param;
    pthread_attr_t attr;

    // Pthreads variables
    pthread_t nrt_loop;

    /*****************************/
    /*   MAIN THREAD ATTRIBUTES  */
    /*****************************/

    sched_getparam(0, &param);
    printf("Priority of the main process: %d\n\r", param.sched_priority);
    pthread_getschedparam(pthread_self(), &policy, &param);
    printf("Priority of the main thread: %d, current policy is: %d\n\r",
              param.sched_priority, policy);

    // Check permission
    policy = SCHED_OTHER;
    if (pthread_setschedparam( pthread_self(),policy, &prio )){
            perror ("Error: RT pthread_setschedparam (root permission?)");
            exit(1);
        }

    /************************/
    /*   THREAD ATTRIBUTES  */
    /************************/

    // Set explicit attributes for thread creation

    pthread_attr_init( &attr);
    pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED);


    // Set scheduler policy

    policy = SCHED_RR;
    pthread_attr_setschedpolicy( &attr, policy);


    // Set scheduler priority


    prio.sched_priority = 30;// -(priority+1) priority range should be btw 1 and 50
    pthread_attr_setschedparam(&attr,&prio);


    // Create thread
    if ( pthread_create(&nrt_loop, &attr, thread, nullptr) ){
        perror( "Error: pthread_create") ;
        return 1;
    }

    /* wait for threads to finish */
    pthread_join(nrt_loop,NULL);

    return 0;
}

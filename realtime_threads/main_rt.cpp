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

#ifdef _POSIX_PRIORITY_SCHEDULING
#define POSIX "POSIX 1003.1b\n";
#endif
#ifdef _POSIX_THREADS
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
#define POSIX "POSIX 1003.1c\n";
#endif
#endif

#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include "time_spec_operation.h"
#include "loop_time_stats.h"

#define DEFAULT_LOOP_TIME_NS 1000000L
#define DEFAULT_APP_DURATION_COUNTS 10000
#define ALLOWED_LOOPTIME_OVERFLOW_NS 100000L

using namespace std;

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

void* rt(void* arg){

    loop_time_stats realtime_loop_time_stats("realtime_loop_time_stats.txt",loop_time_stats::output_mode::screenout_only);

    struct timespec t_start, t_now, t_next,t_period,t_result,t_overflow; // timespec variable to handle timing
    unsigned long int loop_count = 0;   // counter for cycle loops

    // Lock memory to disable paging
    if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        perror("mlockall");
    }

    set_latency_target();

    getinfo();

    // Calculate the time period for the execution of this task
    t_period.tv_sec = 0;
    t_period.tv_nsec = DEFAULT_LOOP_TIME_NS;

    // Get starting time
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    clock_gettime( CLOCK_MONOTONIC, &t_now);
    t_next = t_now;

    /* Cyclic Loop */
    while(loop_count<DEFAULT_APP_DURATION_COUNTS){

        // Save timing stats
        realtime_loop_time_stats.loop_starting_point();



        // Compute next cycle deadline
        timespec_add_nsec(&t_next,&t_next,DEFAULT_LOOP_TIME_NS);

        // Other options to increment
//        TIMESPEC_INCREMENT ( t_next, t_period );

        // Debug cycle at 1 Hz
        if(loop_count%1000==0){

            // Compute time since start
            timespec_sub(&t_result,&t_now,&t_start);
            PLOGI << "RT Clock: " << (double) (timespec_to_nsec(&t_result)/1e9) << endl;

            // Run my cyclic task
            // Do work here
        }
        loop_count++;

        // Sleep until the next execution time
        clock_nanosleep ( CLOCK_MONOTONIC, TIMER_ABSTIME, &t_next, nullptr );
        // Get time after sleep
        clock_gettime ( CLOCK_MONOTONIC, &t_now);
        // Compute overflow time
        timespec_sub(&t_overflow,&t_now,&t_next);
        // If overflow is too big, print overrun
        if(t_overflow.tv_nsec > ALLOWED_LOOPTIME_OVERFLOW_NS)
        {
            PLOGE << "RT Overflow: " << (double)t_overflow.tv_nsec/1000 << endl;
        }

    }

    // Print histogram on screen
    realtime_loop_time_stats.store_loop_time_stats();
}

int main()
{
    // Plogger initialization
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender; // Create appender.
    plog::init(plog::info,&consoleAppender); // Set plogger level.

    printf("Max priority for SCHED_OTHER %d\n", sched_get_priority_max(SCHED_OTHER));
    printf("Min priority for SCHED_OTHER %d\n", sched_get_priority_min(SCHED_OTHER));
    printf("Max priority for SCHED_FIFO %d\n", sched_get_priority_max(SCHED_FIFO));
    printf("Min priority for SCHED_FIFO %d\n", sched_get_priority_min(SCHED_FIFO));
    printf("Max priority for SCHED_RR %d\n", sched_get_priority_max(SCHED_RR));
    printf("Min priority for SCHED_RR %d\n", sched_get_priority_min(SCHED_RR));

    // Scheduler variables
    int policy;
    struct sched_param prio;
    pthread_attr_t attr;

    // Pthreads variables
    pthread_t rt_loop;

    // Check permission
    policy = SCHED_OTHER;
    if (pthread_setschedparam( pthread_self(),policy, &prio )){
            perror ("Error: RT pthread_setschedparam (root permission?)");
            exit(1);
        }

    /********************/
    /* REAL-TIME THREAD */
    /********************/

    // Set explicit attributes for thread creation
    pthread_attr_init( &attr);
    pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED);

    // Set scheduler policy
    policy = SCHED_RR;
    pthread_attr_setschedpolicy( &attr, policy);

    // Set scheduler priority
    // A static priority value is assigned to each process and scheduling depends on this static priority.
    // Processes scheduled with SCHED_OTHER have static priority 0;
    // Processes scheduled under SCHED_FIFO or SCHED_RR can have a static priority in the range 1 to 99.

    prio.sched_priority = 50 ; // priority range should be btw 1 and 99
    pthread_attr_setschedparam(&attr,&prio);

    if ( pthread_create(&rt_loop, &attr, rt, nullptr ) ){
        perror( "Error: RT pthread_create") ;
        return 1;
    }

    /* wait for threads to finish */
    pthread_join(rt_loop,NULL);

    return 0;
}

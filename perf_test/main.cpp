
#include <iostream>
#include <x86intrin.h>
#include <sched.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <chrono>
#include <xmmintrin.h> // SSE

int main()
{  
    int i;
    int no = 1000;
    unsigned long long timestamp[no];
    struct timespec ts[no];

    // add cpu 0 to cpu set
    int cpu_core_id = 0;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core_id, &cpuset);

    // get process id
    pid_t pid = getpid();
    std::cout << pid << std::endl;

    // set cpu affinity for current process
    if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpuset) == -1)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return 1;
    }

    /* struct timespec ts[no];
    ts[0] = timespec();

    for (int i = 0; i < no; i++)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts[i]);
    }  */

    // prefetch data to L1 cache 
    for (i = 0; i < no; i += 8) // + 64-byte cache line
        _mm_prefetch(timestamp + i, _MM_HINT_T0);
    //__builtin_prefetch(timestamp, 1, 1);
    //__builtin_prefetch(timestamp + 8, 1, 1);

    // record CPU timestamps
    unsigned int cpu_id;
    timestamp[0] = __rdtscp(&cpu_id);
    std::cout << cpu_id << std::endl;
    //timestamp[1] = __rdtsc();
    for (i = 0; i < no; i++)
    {
        timestamp[i] = __rdtscp(&cpu_id);
        //clock_gettime(CLOCK_MONOTONIC, &ts[i]);
        //timestamp[i] = ts[i].tv_sec * 1e9 + ts[i].tv_nsec;
    }

    // print timestamps difference
    for (i = 1; i < no; i++)
    {
        std::cout << "CPU Timestamp diff: " << timestamp[i] - timestamp[i - 1] << std::endl;
    }

    return 0;
}
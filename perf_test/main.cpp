#include <iostream>
#include <x86intrin.h>
#include <sched.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <chrono>
#include <xmmintrin.h> // SSE
#include <fstream>
#include <string>

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
    std::cout << "Process ID: " << pid << std::endl;

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

    // record current CPU frequency
    std::string sCPUFreqkHz;
    std::ifstream cpu_freq_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    if (cpu_freq_file.is_open())
    {
        cpu_freq_file >> sCPUFreqkHz;
        std::cout << "Start CPU frequency: " << sCPUFreqkHz << " kHz" << std::endl;
    }

    // prefetch data to L1 cache 
    for (i = 0; i < no; i += 8) // + 64-byte cache line
        _mm_prefetch(timestamp + i, _MM_HINT_T0);
    //__builtin_prefetch(timestamp, 1, 1);
    //__builtin_prefetch(timestamp + 8, 1, 1);

    // record CPU timestamps
    unsigned int cpu_id;
    timestamp[0] = __rdtscp(&cpu_id);
    std::cout << "CPU ID: " << cpu_id << std::endl;
    //timestamp[1] = __rdtsc();
    
    for (i = 0; i < no; i++)
    {
        timestamp[i] = __rdtscp(&cpu_id);
        //clock_gettime(CLOCK_MONOTONIC, &ts[i]);
        //timestamp[i] = ts[i].tv_sec * 1e9 + ts[i].tv_nsec;
    }

    if (cpu_freq_file.is_open())
    {
        // Reset the file pointer to the beginning of the file
        cpu_freq_file.clear(); // Clear the error flags
        cpu_freq_file.seekg(0); // Reset the file pointer to the beginning
        cpu_freq_file >> sCPUFreqkHz;
        std::cout << "End CPU frequency: " << sCPUFreqkHz << " kHz" << std::endl;
    }

    // print timestamps difference
    unsigned long long sum = 0;
    for (i = 1; i < no; i++)
    {
        unsigned long long diff = timestamp[i] - timestamp[i - 1];
        sum += diff;
        std::cout << "CPU Timestamp diff: " << diff << std::endl;
    }
    unsigned long long mean = sum / (no - 1);
    std::cout << "CPU Timestamp diff mean: " << mean << std::endl;
    // constant TSC frequency being 2688 MHz for my case
    std::cout << "CPU Timestamp diff mean (ns): " << 1'000'000'000.0 / 2'688'000'000 * mean  << std::endl;

    return 0;
}
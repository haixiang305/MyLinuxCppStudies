#include <iostream>
#include <x86intrin.h>
#include <sched.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <chrono>
#pragma GCC target("sse4.2")
#include <xmmintrin.h> // SSE
#include <fstream>
#include <string>
#include <vector>

constexpr int func (int n) { return n; }

int i;
const int no = 1000;
unsigned long long timestamp[func(no)];
//struct timespec ts[no];
std::vector<int> a, b, c;

inline void __attribute__((always_inline)) RecordCPUTimestamps()
{
    // reserve memory space at data segment
    memset(timestamp, 0, sizeof(timestamp));

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
}

inline void __attribute__((always_inline)) PrintCPUTimestamps()
{
    // print timestamps difference
    unsigned long long sum = 0;
    for (i = 1; i < no; i++)
    {
        unsigned long long diff = timestamp[i] - timestamp[i - 1];
        sum += diff;
        std::cout << i << " CPU Timestamp diff: " << diff << std::endl;
    }
    unsigned long long mean = sum / (no - 1);
    std::cout << "CPU Timestamp diff mean: " << mean << std::endl;
    // constant TSC frequency being 2688 MHz for my case
    std::cout << "CPU Timestamp diff mean (ns): " << 1'000'000'000.0 / 2'688'000'000 * mean  << std::endl;
}

void MultiplyWithSIMD()
{
    // Initialize a, b, c
    a.resize(no);
    b.resize(no);
    c.resize(no);
    for (i = 0; i < no; i++) {
        a[i] = i;
        b[i] = i * 2;
    }

    // SIMD multiplication
    __m128i* pa = reinterpret_cast<__m128i*>(a.data());
    __m128i* pb = reinterpret_cast<__m128i*>(b.data());
    __m128i* pc = reinterpret_cast<__m128i*>(c.data());

    for (i = 0; i < no / 4; i++) 
    {
        __m128i va = _mm_loadu_si128(pa + i);
        __m128i vb = _mm_loadu_si128(pb + i);
        __m128i vc = _mm_mullo_epi32(va, vb);
        _mm_storeu_si128(pc + i, vc);
    }
    
    for (i = 0; i < no; i++)
    {
        std::cout << i << " SIMD result: " << c[i] << std::endl;
    }
}

int main()
{  
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

    RecordCPUTimestamps();

    if (cpu_freq_file.is_open())
    {
        // Reset the file pointer to the beginning of the file
        cpu_freq_file.clear(); // Clear the error flags
        cpu_freq_file.seekg(0); // Reset the file pointer to the beginning
        cpu_freq_file >> sCPUFreqkHz;
        std::cout << "End CPU frequency: " << sCPUFreqkHz << " kHz" << std::endl;
    }

    PrintCPUTimestamps();

    MultiplyWithSIMD();

    return 0;
}
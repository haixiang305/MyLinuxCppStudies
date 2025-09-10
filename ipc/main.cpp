#include <iostream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

// Shared memory structure
struct SharedMemory 
{
    int data;
};

int main() 
{
    // Create a shared memory segment
    int fd = shm_open("/shared_memory", O_RDWR | O_CREAT, 0666);
    if (fd == -1) 
    {
        perror("shm_open");
        return 1;
    }

    // Set the size of the shared memory segment
    if (ftruncate(fd, sizeof(SharedMemory)) == -1) 
    {
        perror("ftruncate");
        return 1;
    }

    // Map the shared memory segment
    SharedMemory* shm = (SharedMemory*)mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) 
    {
        perror("mmap");
        return 1;
    }

    // Fork a new process
    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("fork");
        return 1;
    }

    if (pid == 0) 
    {
        // Child process
        std::cout << "Child process: reading from shared memory..." << std::endl;
        std::cout << "Value: " << shm->data << std::endl;
        // Write to the shared memory
        shm->data = 24;
        std::cout << "Child process: wrote to shared memory: " << shm->data << std::endl;
    } 
    else
    {
        // Parent process
        std::cout << "Parent process: writing to shared memory..." << std::endl;
        shm->data = 42;
        std::cout << "Parent process: wrote to shared memory: " << shm->data << std::endl;
        // Wait for the child process to finish
        wait(NULL);
    }

    // Unmap the shared memory segment
    if (munmap(shm, sizeof(SharedMemory)) == -1) 
    {
        perror("munmap");
        return 1;
    }

    // Close the shared memory file descriptor
    if (close(fd) == -1) 
    {
        perror("close");
        return 1;
    }

    // Remove the shared memory segment
    if (shm_unlink("/shared_memory") == -1) 
    {
        perror("shm_unlink");
        return 1;
    }

    return 0;
}
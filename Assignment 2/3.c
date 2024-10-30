#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define SHARED_MEMORY_SIZE sizeof(char[10])

sem_t *wrt;
pthread_mutex_t *mutex;
char *cnt;
int *num_active_writers;

void *writer(void *wno) {
    sem_wait(wrt);
    int num = atoi(cnt);
    num *= 2;
    sprintf(cnt, "%d", num);
    printf("Writer %d modified cnt to %s\n", *((int*)wno), cnt);
    sem_post(wrt);

    // Decrement the number of active writer processes
    pthread_mutex_lock(mutex);
    (*num_active_writers)--;
    pthread_mutex_unlock(mutex);

    return NULL;
}
void *reader(void *rno) {
    while(*num_active_writers > 0) {
        pthread_mutex_lock(mutex);
        int numreader = atoi(cnt);
        numreader++;
        if (numreader == 1)
            sem_wait(wrt);
        pthread_mutex_unlock(mutex);

        printf("Reader %d: read cnt as %s\n", *((int*)rno), cnt);

        pthread_mutex_lock(mutex);
        numreader = atoi(cnt);
        numreader--;
        if (numreader == 0)
            sem_post(wrt);
        pthread_mutex_unlock(mutex);
        sleep(1);
    }
}

//signal handler for CTRL+C
void signal_handler(int signal) {
    printf("\nCTRL+C detected. Terminating all processes and threads\n");
    sem_destroy(wrt);
    pthread_mutex_destroy(mutex);
    munmap(wrt, SHARED_MEMORY_SIZE);
    munmap(mutex, sizeof(pthread_mutex_t));
    munmap(cnt, SHARED_MEMORY_SIZE);
    munmap(num_active_writers, sizeof(int));
    exit(1);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    if (argc != 3) {
        printf("Proper syntax: %s <number of writer processes> <number of reader threads>\n", argv[0]);
        exit(1);
    }

    int num_writer_processes = atoi(argv[1]);
    int num_reader_threads = atoi(argv[2]);

    wrt = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mutex = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    cnt = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    num_active_writers = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_init(wrt, 1, 1);
    pthread_mutex_init(mutex, NULL);
    sprintf(cnt, "%d", 1);
    *num_active_writers = num_writer_processes;

    pid_t pid;
    for (int i = 0; i < num_writer_processes; i++) {
        pid = fork();
        if (pid == 0) {
            writer((void*)&i);
            exit(0);
        }
    }

    pthread_t read[num_reader_threads];

    int reader_args[num_reader_threads];
    for (int i = 0; i < num_reader_threads; i++) {
        reader_args[i] = i + 1;
        pthread_create(&read[i], NULL, reader, (void *)&reader_args[i]);
    }

    for (int i = 0; i < num_reader_threads; i++) {
        pthread_join(read[i], NULL);
    }

    sem_destroy(wrt);
    pthread_mutex_destroy(mutex);
    munmap(wrt, SHARED_MEMORY_SIZE);
    munmap(mutex, sizeof(pthread_mutex_t));
    munmap(cnt, SHARED_MEMORY_SIZE);
    munmap(num_active_writers, sizeof(int));

    return 0;
}

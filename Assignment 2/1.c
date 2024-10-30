#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

/*
This program provides a solution for the first readers-writers problem using mutex and semaphore.
It ensures that the number of active readers is upper-bounded by a specified value N.
*/

sem_t wrt, mutex, readCountLock;
int cnt = 1;
int numreader = 0;
int N = 5; // Define the maximum number of readers

void *writer(void *wno)
{
    sem_wait(&wrt);
    cnt = cnt * 2;
    printf("Writer %d modified cnt to %d\n", (*((int *)wno)), cnt);
    sem_post(&wrt);
}

void *reader(void *rno)
{
    sem_wait(&readCountLock);
    if (numreader == N) {
        sem_post(&readCountLock);
        return NULL; // Return if the maximum number of readers is reached
    }
    numreader++;
    if (numreader == 1) {
        sem_wait(&wrt); // If this is the first reader, block the writer
    }
    sem_post(&readCountLock);

    // Reading Section
    printf("Reader %d: read cnt as %d\n", *((int *)rno), cnt);

    sem_wait(&readCountLock);
    numreader--;
    if (numreader == 0) {
        sem_post(&wrt); // If this is the last reader, wake up the writer
    }
    sem_post(&readCountLock);
}

int main()
{
    pthread_t read[10], write[5];
    sem_init(&wrt, 0, 1);
    sem_init(&mutex, 0, 1);
    sem_init(&readCountLock, 0, 1); // Semaphore to control access to numreader

    int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // Used for numbering the readers and writers

    for (int i = 0; i < 10; i++) {
        pthread_create(&read[i], NULL, (void *)reader, (void *)&a[i]);
    }
    for (int i = 0; i < 5; i++) {
        pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(read[i], NULL);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(write[i], NULL);
    }

    sem_destroy(&wrt);
    sem_destroy(&mutex);
    sem_destroy(&readCountLock);

    return 0;
}

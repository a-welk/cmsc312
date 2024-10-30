/*Alex Welk - CMSC 312 Assignment 2 Question 2*/
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
/*
This program provides a possible solution for first readers writers problem using 
mutex and semaphore.
I have used 10 readers and 5 producers to demonstrate the solution. You can always 
play with these values.
*/
int min(int a, int b){
    return(a > b) ? b : a;
}
struct CSEM {
    int val;
    sem_t gate;
    sem_t mutex;
}; 
void Pc(struct CSEM *cs){
    sem_wait(&(cs->gate));
    sem_wait(&(cs->mutex));
    cs->val--;
    if(cs->val > 0){
        sem_post(&(cs->gate));
    }
    sem_post(&(cs->mutex));
    }
void Vc(struct CSEM *cs){
    sem_wait(&(cs->mutex));
    cs->val++;
    if(cs->val == 1){
        sem_post(&(cs->gate));
    }
    sem_post(&(cs->mutex));
}
struct CSEM write;
int n = 3;
pthread_mutex_t mutex;
int cnt = 1;
int numReader = 0;
void *writer(void *wno)
{   
    Pc(&write);
    cnt = cnt*2;
    printf("Writer %d modified cnt to %d\n",(*((int *)wno)),cnt);
    Vc(&write);
}
void *reader(void *rno)
{   
    // Reader acquire the lock before modifying number of readers
    pthread_mutex_lock(&mutex);
    numReader++;
    if(numReader == 1) {
        Pc(&write); // If this is the first reader, then it will block writer
    }
    pthread_mutex_unlock(&mutex);
    // Reading Section
    Pc(&write);
    printf("Reader %d: read cnt as %d\n",*((int *)rno),cnt);
    
    Vc(&write);
    // Reader acquires the lock before modifying numReader
    pthread_mutex_lock(&mutex);
    numReader--;
    if(numReader == 0) {
        Vc(&write); // If this is the last reader, it will wake up the writer.
    }
    pthread_mutex_unlock(&mutex);
}
int main()
{   
    pthread_t read[10],write[5];
    pthread_mutex_init(&mutex, NULL);

    write.val = n;
    sem_init(&(write.gate), 0, min(1, write.val));
    sem_init(&(write.mutex), 0, 1);

    int a[10] = {1,2,3,4,5,6,7,8,9,10}; //Just used for numbering the producer and consumer
    for(int i = 0; i < 10; i++) {
        pthread_create(&read[i], NULL, (void *)reader, (void *)&a[i]);
    }
    for(int i = 0; i < 5; i++) {
        pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]);
    }
    for(int i = 0; i < 10; i++) {
        pthread_join(read[i], NULL);
    }
    for(int i = 0; i < 5; i++) {
        pthread_join(write[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&(write.gate));
    sem_destroy(&(write.mutex));
    return 0;
    
}
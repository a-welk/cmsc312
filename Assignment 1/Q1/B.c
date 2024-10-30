
/*
 * shm-client - client program to demonstrate shared memory.
 * shm-client - not sure of the origin of these code segments.
 * possibly: David Marshalls course : http://www.cs.cf.ac.uk/Dave/C/CE.html
 * or Steve Holmes : http://www2.its.strath.ac.uk/courses/c/
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define SHMSZ     27

int
main()
{
    char c;
    int shmid_pid, shmid_char, shmid_str;
    key_t key_pid, key_char, key_str;
    char *shm_pid, *shm_char, *shm_str;

    key_pid = 5678;

    /*
     * Create the segment.
     */
    if( (shmid_pid = shmget(key_pid, SHMSZ, IPC_CREAT | 0666)) < 0 )
    {
        perror("shmget");
        exit(1);
    }
    /*
     * Now we attach the segment to our data space.
     */
    if( (shm_pid = shmat(shmid_pid, NULL, 0)) == (char *) -1 )
    {
        perror("shmat");
        exit(1);
    }

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    //*shm_pid = getpid();


    //create second segment
    key_char = 4968;
    if( (shmid_char = shmget(key_char, SHMSZ, IPC_CREAT | 0666)) < 0 )
    {
        perror("shmget");
        exit(1);
    }
    /*
     * Now we attach the segment to our data space.
     */
    if( (shm_char = shmat(shmid_char, NULL, 0)) == (char *) -1 )
    {
        perror("shmat");
        exit(1);
    }
    //*shm_char = 'A';

    //create third segment
    key_str = 1290;
    if( (shmid_str = shmget(key_str, SHMSZ, IPC_CREAT | 0666)) < 0 )
    {
        perror("shmget");
        exit(1);
    }
    /*
     * Now we attach the segment to our data space.
     */
    if( (shm_str = shmat(shmid_str, NULL, 0)) == (char *) -1 )
    {
        perror("shmat");
        exit(1);
    }

    while(*shm_char != 'A'){
        sleep(1); }
    
    *shm_pid = getpid();
    sprintf(shm_str, "I am process B - %d", getpid());
    *shm_char = 'B';
    /*
     * Now read what the server put in the memory.
     
    for( s = shm; *s != (char) NULL; s++ )
        putchar(*s);
    putchar('\n');
*/
    /*
     * Finally, change the first character of the 
     * segment to '*', indicating we have read 
     * the segment.
     

    shmdt(shm_char);
    shmdt(shmid_char);
    shmdt(key_char);
    shmctl(shmid_char, IPC_RMID, NULL);
    shmctl(shmid_pid, IPC_RMID, NULL);
    shmctl(shmid_str, IPC_RMID, NULL);*/


    return 0;
}

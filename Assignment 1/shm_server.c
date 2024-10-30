#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h> /* for exit */


/* 
 * shm-server - not sure of the origin of code segment.
 * Old source, not sure of the origin
 *  possibly: David Marshalls course : http://www.cs.cf.ac.uk/Dave/C/CE.html
 * or Steve Holmes : http://www2.its.strath.ac.uk/courses/c/
 */



#define SHMSZ     27


int 
main()
{
    char c;
    int shmid1, shmid2, shmid3;
    key_t key1, key2, key3;
    char *shm, *s;

    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key1 = 5678;

    /*
     * Create the segment.
     */
    if( (shmid1 = shmget(key1, SHMSZ, IPC_CREAT | 0666)) < 0 )
    {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if( (shm = shmat(shmid1, NULL, 0)) == (char *) -1 )
    {
        perror("shmat");
        exit(1);
    }

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    *shm = getpid();

    //create second segment
    key2 = 4968
    if( (shmid2 = shmget(key2, SHMSZ, IPC_CREAT | 0666)) < 0 )
    {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if( (shm = shmat(shmid2, NULL, 0)) == (char *) -1 )
    {
        perror("shmat");
        exit(1);
    }

    *shm = 'A';

    //create third segment
    key3 = 1290
    if( (shmid3 = shmget(key3, SHMSZ, IPC_CREAT | 0666)) < 0 )
    {
        perror("shmget");
        exit(1);
    }
    
    *shm = "I am Process A - %s", getpid()

    /*
     * Now we attach the segment to our data space.
     */
    if( (shm = shmat(shmid3, NULL, 0)) == (char *) -1 )
    {
        perror("shmat");
        exit(1);
    }

    /*
     * Finally, we wait until the other process 
     * changes the first character of our memory
     * to '*', indicating that it has read what 
     * we put there.
     */
    while( *shm != '*' )
        sleep(1);

    
    key = 5768;

    /*
     * Create the segment.
     */
    if( (shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0 )
    {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if( (shm = shmat(shmid, NULL, 0)) == (char *) -1 )
    {
        perror("shmat");
        exit(1);
    }

    return 0;
}

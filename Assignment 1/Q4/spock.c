/*
** spock.c -- reads from a message queue
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct my_msgbuf {
	long mtype;
	char mtext[200];
};

void bubbleSort(int nums[], int size) {
    int i, j;
    for (i = 0; i < size-1; i++) {
        for (j = 0; j < size-i-1; j++) {
            if (nums[j] > nums[j+1]) {
                // swap nums[j] and nums[j+1]
                int temp = nums[j];
                nums[j] = nums[j+1];
                nums[j+1] = temp;
            }
		}
	}
}

int main(void)
{
	struct my_msgbuf buf;
	int msqid;
	key_t key;

	if ((key = ftok("kirk.c", 'B')) == -1) {  /* same key as kirk.c */
		perror("ftok");
		exit(1);
	}

	if ((msqid = msgget(key, 0644)) == -1) { /* connect to the queue */
		perror("msgget");
		exit(1);
	}
	
	printf("spock: ready to receive messages, captain.\n");

	for(;;) { /* Spock never quits! */
		if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) {
			perror("msgrcv");
			exit(1);
		}

		int nums[200];
		int count = 0;
		char* token = strtok(buf.mtext, " ");
		
		while(count < 200 && token != NULL) {
			nums[count++] = atoi(token);
			token = strtok(NULL, " ");
		}

		//sort array
		bubbleSort(nums, count);

		strcpy(buf.mtext, "");
		for(int i = 0; i < count; i++) {
			char temp[10];
			sprintf(temp, "%d ", nums[i]);
			strcat(buf.mtext, temp);
		}
		printf("spock: %s\n", buf.mtext);
	}

	return 0;
}


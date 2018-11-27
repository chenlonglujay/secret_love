#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>
#include <mutex>

#define MY_SHM_ID 8787
#define BUF_SIZE 8
std::mutex mtx;

void get_buf(char *buf){
	int i=0;
	while((buf[i]=getchar())!='\n' && i <1024)
		i++;
}

int main() {
	printf("page size=%d\n", getpagesize());
	int shmid=0, ret=0;
	shmid = shmget(MY_SHM_ID, BUF_SIZE, 0666|IPC_CREAT);
	if(shmid > 0) {
		printf("Create a shared memory segment %d\n", shmid);
	}

	struct shmid_ds shmds;
	ret = shmctl( shmid, IPC_STAT, &shmds);
	
	if(ret == 0) {
		printf("Size of memory segment is %d \n", shmds.shm_segsz);
	 	printf("Number of attaches %d \n", (int)shmds.shm_nattch);
	} else {
		printf("shmctl () call failed \n");
	}

	//write data to share memory
	char *buf = NULL;
	if((int)(buf=(char*)shmat(shmid, NULL, 0)) == -1) {
		perror("Share memory can't get pointer\n");
	}

	if(mtx.try_lock()) {
		get_buf(buf);
		mtx.unlock();
	}

#if 0
	//sleep(30);
	//delete share memory
	ret = shmctl(shmid, IPC_RMID, 0);		
	if(ret == 0) {
		printf("Shared memory removed \n");
	} else {
		printf("Shared memory remove failed\n");
	}
#endif
	return 0;
}






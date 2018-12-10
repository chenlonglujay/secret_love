#include "send_I2C_command.h"

std::mutex mtx;
send_I2C_command::send_I2C_command() {
	buf = NULL;
}

send_I2C_command::~send_I2C_command() {
	//if this process creates share memory,when process is killed,
	//it needs to delete share memory
	delete_share_memory();
}

void send_I2C_command::delete_share_memory() {
	int ret = shmctl(shmid, IPC_RMID, 0);		
	if(ret == 0) {
		printf("Shared memory removed \n");
	} else {
		printf("Shared memory remove failed\n");
	}

}

bool send_I2C_command::create_share_memory(int SHM_ID) {
	int ret=0;
	shmid = SHM_ID;
	printf("page size=%d\n", getpagesize());
	shmid = shmget(shmid, BUF_SIZE, 0666|IPC_CREAT);
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
		return create_fail; 
	}
	 
	//write data to share memory
	if((int)(buf=(char*)shmat(shmid, NULL, 0)) == -1) {
			perror("Share memory can't get pointer\n");
	}
	return create_ok; 
}

void send_I2C_command::send_data_to_shm(char *command) {
		//printf("send_data_to_shm\n");
		int length = strlen(command);
		//printf("key in String length: %d\n", length);
		if(length == 6) {
			if(mtx.try_lock()) {
				strcpy(buf, command);
			}
			mtx.unlock();
		}
}


void send_I2C_command::send_data_to_shm_keyin() {
	int i = 0;
	printf("please key in number (data format: nnnnnn),press enter will exit\n");
	if(mtx.try_lock()) {
		while((buf[i]=getchar())!='\n' && i < 1024)
			i++;
	}
	mtx.unlock();
}





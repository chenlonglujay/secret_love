/*
sends I2C command to share memory,for python process to get I2C command from share memory
and transmits I2C data to arduino Mega 2560
*/

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <mutex>
#include <stdio.h>
#include <string.h>
#define BUF_SIZE 8
#define create_ok 1
#define create_fail 0

class send_I2C_command {
	private:
		int shmid;
		char *buf;
		void delete_share_memory();
	public:
		send_I2C_command();
		~send_I2C_command();
		bool create_share_memory(int SHM_ID);
		void send_data_to_shm(char *command);
		void send_data_to_shm_keyin();
};	




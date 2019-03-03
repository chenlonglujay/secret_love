#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "send_I2C_command.h"
#include <string.h>
#include <signal.h>
#include <sys/time.h>

using namespace std; 
send_I2C_command SIC;
#define test_jog_mode 1
void test_jog_mode_fn0();
void test_jog_mode_fn1();
bool timer_initial(struct itimerval *src);
void timer_event(int a);
int seq;
int main(int argc,char **argv) {
	int shmid = 8787;
	char command[6];
	struct itimerval timer;
	if(!timer_initial(&timer)) {
			return -1;
	}
	memset(command,'0', sizeof(command));
	printf("command length: %d\n",sizeof(command));
	printf("command: %s\n",command);

	if(argc > 0) {
		printf("key in:%s\n", argv[1]);
	}
	
	if(!SIC.create_share_memory(shmid)) {
		printf("create share memory fail!!\n");
	}
	
	strcpy(command, "000000");
	SIC.send_data_to_shm(command);
	//printf("command: %s\n",command);
	
	while(1) {
		try
		{
			if(!strcmp(argv[1], "0")) {
				SIC.send_data_to_shm_keyin();
			} else if(!strcmp(argv[1], "1")) {
							
#if test_jog_mode
				test_jog_mode_fn0();
#else 
				test_jog_mode_fn1();
#endif
			}
		} catch(std::exception& e) {
			printf("%s\n", e);
		}
	}
	return 0;
}


void test_jog_mode_fn0() {
	char command[6];
	int i = 0;
	if(seq == 0) {
		strcpy(command, "070001");		//Motor Enable
		SIC.send_data_to_shm(command);
	}else if(seq == 1) {
		strcpy(command, "010000");		//CW
		SIC.send_data_to_shm(command);
  } else if (seq == 2) {
		strcpy(command, "030000");		//jog
		SIC.send_data_to_shm(command);		
	} else if (seq == 3) {
		strcpy(command, "010001");		//CCW
		SIC.send_data_to_shm(command);
	} else if(seq == 4) {
		strcpy(command, "040000");		//stop
		SIC.send_data_to_shm(command);			
	}	
}

void test_jog_mode_fn1() {
	char command[6];
	int i = 0;
	if(seq == 0) {
		strcpy(command, "070001");		//Motor Enable
		SIC.send_data_to_shm(command);
	}else if(seq == 1) {
		strcpy(command, "010000");		//CW
		SIC.send_data_to_shm(command);
  } else if (seq == 2) {
		strcpy(command, "030000");		//jog
		SIC.send_data_to_shm(command);		
	} 
}

bool timer_initial(struct itimerval *src) {

	src->it_interval.tv_usec = 0;
	src->it_interval.tv_sec = 3;
	src->it_value.tv_usec = 0;
	src->it_value.tv_sec = 1;

	if( setitimer( ITIMER_REAL, src, NULL) < 0 ){
		printf("set timer failed!\n");		
		return 0;
	}
	signal( SIGALRM, timer_event );
	return 1;
}

void timer_event(int a) {
	//printf("timer!!\n");
	if(seq == 0) {
		seq++;
	} else if(seq == 1) {
		seq++;
	}	else if(seq == 2) {
		seq++;
	} else if(seq == 3) {
		seq++;
	} else if(seq == 4) {
		seq = 1;		//doesn't execute motor enable again
	}

}

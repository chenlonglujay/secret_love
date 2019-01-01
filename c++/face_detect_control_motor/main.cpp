#include <iostream>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include "../send_I2C_command.h" 
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>
#include "motor_control.h"


#define action_log false //false = print
using namespace cv ;
using namespace std ;

pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
send_I2C_command SIC;
motor_control MC;
void request_type_ok();
void timer_initial(int interval, int when_start);
void signal_initial();
void pthread_initial(pthread_t *timer, pthread_t *action);
static void* timer_threadproc(void *thratg);
static void* action_threadproc(void *thratg);
void send_command(string input);
bool detectAndDisplay( Mat frame, bool eyes_detect );
void next_action_check();
void main_all_detections(Mat frame);
static void sig_alrm_handler(int signal) {
	//dummy signal handler,
	//the signal is actually handled in timer_threadproc()
}

//void stop_timer(struct itimerval *src);

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt2.xml";
//String face_cascade_name = "haarcascade_frontalcatface.xml";
CascadeClassifier face_cascade;
#if 1
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
//String eyes_cascade_name = "haarcascade_eye.xml";
CascadeClassifier eyes_cascade;
#endif

string window_name = "Capture - Face detection";
//RNG rng(12345);
//下載　https://github.com/opencv/opencv/tree/master/data/haarcascades

int main() {
		
		int shmid = 8787;
		char command[6];
		pthread_t timer_thread,action_thread;
		memset(command, '0', sizeof(command));
		if(!SIC.create_share_memory(shmid)) {
			printf("create share memory fail!!\n");
			return -1;
		}		
		
		strcpy(command, motor_nothing);
		SIC.send_data_to_shm(command);
		request_type_ok();	

		signal_initial();
		pthread_initial(&timer_thread, &action_thread);
		timer_initial(timer_interval, 1);	


		//load face detect model
    if(!face_cascade.load(face_cascade_name)) {
        printf("--(!)Error loading\n");
        return -1;
    }
#if 1
    if (!eyes_cascade.load(eyes_cascade_name))
    {
        printf("--(!)Error loading\n");
        return -1;
    }
#endif
    VideoCapture cap(0);             //開啟攝影機
    if(!cap.isOpened()) {
			printf("please turn the camera\n");
			return -1;   //確認攝影機打開
		}
    Mat frame;                       //用矩陣紀錄抓取的每張frame
    while(1) {
        if (cap.isOpened()) {
            cap>>frame;   //把取得的影像放置到矩陣中
            if( !frame.empty() ) {
							pthread_mutex_lock(&mutex0);	
							main_all_detections(frame);
							pthread_mutex_unlock(&mutex0);	
						} else {
							printf(" --(!) No captured frame -- Break!"); 
							break;
						}
            
            if(waitKey(30) >= 0) {
							send_command(motor_stop);
							pthread_join(timer_thread, NULL);
							pthread_join(action_thread, NULL);
							pthread_mutex_destroy(&mutex0);
							break;  //按鍵就離開程式
						}

        }
    }
    system("PAUSE");
    return 0;
}

void request_type_ok() {
	char check[3] ="ok";
	char str[3];
	printf("please execute python I2C_send_command_from_shm.py to receives command\n");
	printf("if you did it ,type ok\n");
	while(strcmp(str,check)!=0) {
		scanf("%s",str);
		printf("str:%s\n", str);
	}
}

void timer_initial(int interval, int when_start) {
	//setup timer		
	struct itimerspec tspec;
	timer_t timer_id;

	tspec.it_interval.tv_sec = 0;
	tspec.it_interval.tv_nsec = interval;
	tspec.it_value.tv_sec = when_start;
	tspec.it_value.tv_nsec = 0;
	timer_create(CLOCK_REALTIME, NULL, &timer_id);
	timer_settime(timer_id, 0, &tspec, NULL);	

}

void signal_initial() {
	sigset_t sigset;
	struct sigaction sa;

	//make SIGALRM in all threads by default
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	//we need a signal handler. the default is to call abort() and
	//setting SIG_IGN might cause the signal to not be delivered at all
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_alrm_handler;
	sigaction(SIGALRM, &sa, NULL);
}

void pthread_initial(pthread_t *timer, pthread_t *action) {
	//pthread_t timer_thread,action_thread;
		
	//create SIGALRM looper thread
	pthread_create(timer/*&timer_thread*/, NULL, timer_threadproc, NULL);

	//create action thread
	pthread_create(action/*&action_thread*/, NULL , action_threadproc, NULL);

}

static void* timer_threadproc(void *arg) {
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);	
	//endless loop to wait for and handle a signal repeatedly
	int sig,error;
	while(!MC.get_all_action_finished_flag()) {
		error = sigwait(&sigset, &sig);
		if(error == 0) {
			assert(sig == SIGALRM);
			pthread_mutex_lock(&mutex0);
			next_action_check();
			pthread_mutex_unlock(&mutex0);
			//printf("got SIGALRM\n");
		} else {
			perror("sigwait");
		}
	}
	//return NULL;
	pthread_exit(NULL);
}

static void* action_threadproc(void *arg) {
	struct send_command sc;
	string cmd;
	while(!MC.get_all_action_finished_flag()) {
		pthread_mutex_lock(&mutex0);
		MC.run_action(&sc);
		pthread_mutex_unlock(&mutex0);
		if(sc.send) {
			cmd = sc.command;
			send_command(cmd);
		}
	}
	printf("action completed\n");
	pthread_exit(NULL);
}



void send_command(string input) {
	char command[7];
	strcpy(command, input.c_str());
	//printf("command: %s\n", input.c_str());
	SIC.send_data_to_shm(command);
}

void next_action_check() {
	if(!MC.get_initial_flag() && MC.get_all_action_seq()==0) {
			//printf("initial\n");
		if(!MC.get_timer_lock_flag()) {
			MC.add_timer_counter();
			if(MC.get_timer_counter() == MC.get_timer_goal()) {
				MC.set_printfed_flag(action_log);
				MC.set_timer_lock_flag(true);
				MC.clean_timer_counter();
				MC.add_initial_seq();
			  printf("get_initial_seq: %d\n", MC.get_initial_seq());
				if(MC.get_initial_seq() == initial_seq_goal) {
					MC.set_initial_flag(true);
					MC.clean_initial_seq();
					MC.set_all_action_seq(1);
					//printf("finish initial\n");
				} 		
			}
		}
	}

	else if(!MC.get_detect_face_flag() && MC.get_all_action_seq()==1) {
		//printf("detect_face\n");
		if(!MC.get_timer_lock_flag()) {
			MC.add_timer_counter();
			if(MC.get_timer_counter() == MC.get_timer_goal()) {
				MC.set_printfed_flag(action_log);
				MC.set_timer_lock_flag(true);
				MC.clean_timer_counter();
				MC.add_detect_face_seq();
			  //printf("get_detect_face_seq: %d\n", MC.get_detect_face_seq());
				if(MC.get_detect_face_seq() == detect_face_seq_goal) {
					MC.clean_detect_face_seq();
				}
			}
		}
	}

	else if(!MC.get_delay_ok_flag() && MC.get_all_action_seq()==2) {
		//printf("delay\n");
		if(!MC.get_timer_lock_flag()) {
			MC.add_timer_counter();
			if(MC.get_timer_counter() == MC.get_timer_goal()) {
				MC.set_printfed_flag(action_log);
				MC.set_timer_lock_flag(true);
				MC.clean_timer_counter();
				MC.set_delay_ok_flag(true);
				MC.set_all_action_seq(3);
			}
		}
	}
	
	else if(!MC.get_detect_eyes_flag() && MC.get_all_action_seq()==3) {
		MC.set_printfed_flag(action_log);
	}

	else if(!MC.get_go_back_flag() && MC.get_all_action_seq()==4) {
		//printf("go_back\n");
		if(!MC.get_timer_lock_flag()) {
			MC.add_timer_counter();
			if(MC.get_timer_counter() == MC.get_timer_goal()) {
				MC.set_printfed_flag(action_log);
				MC.set_timer_lock_flag(true);
				MC.clean_timer_counter();
				MC.add_go_back_seq();
				if(MC.get_go_back_seq() == go_back_seq_goal) {
					MC.set_go_back_flag(true);
					MC.clean_go_back_seq();
					MC.set_all_action_finished_flag(true);
					MC.set_all_action_seq(5);
				}
			}
		}
	} else {
		//printf("nothing\n");
	}
}

void main_all_detections(Mat frame) {
	if(!MC.get_detect_face_flag() && MC.get_all_action_seq()==1) {
	//detect face
		if(detectAndDisplay(frame, false)) {
			send_command(motor_stop);
			MC.set_detect_face_flag(true);	
			MC.set_all_action_seq(2);
			MC.set_timer_lock_flag(true);
			MC.clean_timer_counter();
			MC.clean_detect_face_seq();
			MC.set_printfed_flag(action_log);
			printf("detect face\n");
		} 
	} else if(!MC.get_detect_eyes_flag() && MC.get_all_action_seq()==3) {
	//detect eyes
		if(detectAndDisplay(frame, true)) {
			MC.set_detect_eyes_flag(true);	
		  MC.set_all_action_seq(4);
			MC.set_timer_lock_flag(true);
			MC.clean_timer_counter();
			MC.set_printfed_flag(action_log);
			printf("detect eyes\n");
		}
	}

}


bool detectAndDisplay(Mat frame, bool eyes_detect) {
    vector<Rect>faces;
		float scale = 0.6;
    Mat frame_resize;
    Mat frame_gray;
		bool detect = false;
    //imshow("org", frame);                //建立一個視窗,顯示frame到camera名稱的視窗
		resize(frame, frame_resize, Size(frame.cols*scale, frame.rows*scale),0,0,INTER_LINEAR);
    //imshow("resize", frame_resize);                //建立一個視窗,顯示frame到camera名稱的視窗
    cvtColor(frame_resize,frame_gray,CV_BGR2GRAY);
    equalizeHist(frame_gray,frame_gray);
    face_cascade.detectMultiScale(frame_gray,faces,1.1,2,0, Size(100, 100) );
		if(faces.size() > 0) {
			//printf("detect face\n");
			detect = true;
		}	
    for (int i=0;i<faces.size();i++)
    {
        Point center(faces[i].x+faces[i].width*0.5,faces[i].y+faces[i].height*0.5);
        ellipse(frame_resize,center,Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0);
#if 1 
			if(eyes_detect) {
				detect = false;
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;
        //-- In each face, detect eyes
        //eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 , Size(30, 30) );
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 , Size(5, 5) );
        for( int j = 0; j < eyes.size(); j++ )
        {
            Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            circle( frame_resize, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
        }
				if(eyes.size() > 0) {
					//printf("detect face\n");
					detect = true;
				}
			}
#endif
    }
		moveWindow(window_name, 300 , 100);
    imshow(window_name, frame_resize);                //建立一個視窗,顯示frame到camera名稱的視窗

#if 0
		//resize to origin image,but system's speed will become too slow
		resize(frame_resize, frame, Size(frame_resize.cols/scale, frame_resize.rows/scale),0,0,INTER_LINEAR);
    imshow(window_name, frame);                //建立一個視窗,顯示frame到camera名稱的視窗
#endif
    return detect;
}



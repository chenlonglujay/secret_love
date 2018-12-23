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
#include <sys/time.h>
#include "motor_control.h"
using namespace cv ;
using namespace std ;

send_I2C_command SIC;
bool timer_initial(struct itimerval *src,int next, int interval);
void timer_event(int a);
void motor_initial();
void send_command(string input);
void motor_run();
void stop_timer(struct itimerval *src);

bool detectAndDisplay( Mat frame );
/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
#if 0
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier eyes_cascade;
#endif

string window_name = "Capture - Face detection";
//RNG rng(12345);
//下載　https://github.com/opencv/opencv/tree/master/data/haarcascades

char command[6];
struct motor_initial_state MTR_ST;
struct detect_state DCT_ST;
struct motor_run_state MTR_RUN;

int main() {
		
		int shmid = 8787;
		struct itimerval timer;
		char str[5];	
		setup_motor_initial_state(&MTR_ST);
		setup_detect_state(&DCT_ST);
		setup_motor_run_initial(&MTR_RUN);
		if(!timer_initial(&timer,1, 10000)) {
			printf("timer initial wrong!\n");	
			return -1;
		}
		memset(command, '0', sizeof(command));
		if(!SIC.create_share_memory(shmid)) {
			printf("create share memory fail!!\n");
			return -1;
		}		
		
		strcpy(command, motor_nothing);
		SIC.send_data_to_shm(command);
		printf("please execute python I2C_send_command_from_shm.py to receives command\n");
		printf("if you did it ,type ok\n");
		char check[3] ="ok";
	  while(strcmp(str,check)!=0) {
			scanf("%s",str);
			printf("str:%s\n", str);
		}
		
		while(!MTR_ST.finish) {
			motor_initial();
		}
		

		//load face detect model
    if(!face_cascade.load(face_cascade_name)) {
        printf("--(!)Error loading\n");
        return -1;
    }
#if 0
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
							if(!DCT_ST.face_detect) {
								if(detectAndDisplay( frame )) {
									DCT_ST.face_detect = true;	
									send_command(motor_stop);
									MTR_RUN.finish = true;
									setup_motor_run_initial(&MTR_RUN);
									stop_timer(&timer);
									printf("detect face\n");
								} else {
									//keep doing detect
									motor_run();
								}
							} else {
								imshow("live", frame);                //建立一個視窗,顯示frame到camera名稱的視窗
							}
						} else {
							printf(" --(!) No captured frame -- Break!"); 
							break;
						}
            
            if(waitKey(30) >= 0) {
							send_command(motor_stop);
							break;  //按鍵就離開程式
						}

        }
    }
    system("PAUSE");
    return 0;
}

bool detectAndDisplay( Mat frame )
{
    vector<Rect>faces;
		float scale = 0.5;
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
#if 0 
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;
        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 , Size(30, 30) );
        for( int j = 0; j < eyes.size(); j++ )
        {
            Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
        }
#endif
    }
    imshow(window_name, frame_resize);                //建立一個視窗,顯示frame到camera名稱的視窗
		//resize(frame_resize, frame, Size(frame_resize.cols/scale, frame_resize.rows/scale),0,0,INTER_LINEAR);
    //imshow(window_name, frame);                //建立一個視窗,顯示frame到camera名稱的視窗
    return detect;
}

bool timer_initial(struct itimerval *src,int next, int interval) {
	
	src->it_interval.tv_usec = interval;
	src->it_interval.tv_sec = 0/*interval*/;
	src->it_value.tv_usec = 0;
	src->it_value.tv_sec = next;

	if( setitimer( ITIMER_REAL, src, NULL) < 0 ){
		printf("set timer failed!\n");
		return 0;
	}
	signal( SIGALRM, timer_event );
	return 1;
}

void timer_event(int a) {
	if(!MTR_ST.finish) {
		if(MTR_ST.timer_unlock) {
			MTR_ST.timer_unlock = false;
			MTR_ST.seq = MTR_ST.seq + 1;
		}
	}

	if(MTR_ST.finish && !MTR_RUN.finish) {
		if(MTR_RUN.timer_unlock) {
			MTR_RUN.timer_count = MTR_RUN.timer_count + 1; 
			if(MTR_RUN.timer_count == MTR_RUN.count_goal) {
				MTR_RUN.timer_count = 0;
				MTR_RUN.timer_unlock = false;
				MTR_RUN.seq = MTR_RUN.seq + 1;
				if(MTR_RUN.seq == 4) {
					MTR_RUN.seq = 0;
					printf("timer\n");
				}
			}
		}
	}
}

void motor_initial() {
		switch(MTR_ST.seq) {
			case 0:
				send_command(motor_nothing);
				MTR_ST.timer_unlock = true;
			break;
			case 1:
				send_command(CCW);
				MTR_ST.timer_unlock = true;
				MTR_RUN.change_DIR = true;
			break;
			case 2:
				send_command(motor_spd7);
				MTR_ST.timer_unlock = true;
			break;
			case 3:
				send_command(LED_off);
				MTR_ST.timer_unlock = false;
				MTR_ST.finish = true;
				printf("motor_initial_ok\n");
			break;
			default:
				MTR_ST.timer_unlock = false;
			break;
		};
}

void motor_run() {
	switch(MTR_RUN.seq) {
		case 0:
			if(MTR_RUN.change_DIR) { 
				send_command(CW);
			} else {
				send_command(CCW);
			}
			MTR_RUN.change_DIR = !MTR_RUN.change_DIR;
			MTR_RUN.count_goal = run_count_set;
			MTR_RUN.timer_unlock = true;
		break;
		case 1:
			send_command(motor_spd6);
			MTR_RUN.count_goal = run_count_set;
			MTR_RUN.timer_unlock = true;
		break;
		case 2:
			send_command(motor_steps_1000);
			MTR_RUN.count_goal = run_count_rotate;
			MTR_RUN.timer_unlock = true;
		break;
		case 3:
			send_command(motor_stop);
			MTR_RUN.count_goal = run_count_set;
			MTR_RUN.timer_unlock = true;
		break;
	};
}


void send_command(string input) {
	strcpy(command, input.c_str());
	SIC.send_data_to_shm(command);
}

void stop_timer(struct itimerval *src) {
	timer_initial(src,0,0);
}

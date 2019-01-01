#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#define motor_nothing "000000"
#define motor_jog	"030000"
#define motor_stop "040000"
#define CW "010000"
#define CCW "010001"

//0(fast)-9(slow)
#define motor_spd0 "050000"
#define motor_spd1 "050001"
#define motor_spd2 "050002"
#define motor_spd3 "050003"
#define motor_spd4 "050004"
#define motor_spd5 "050005"
#define motor_spd6 "050006"
#define motor_spd7 "050007"
#define motor_spd8 "050008"
#define motor_spd9 "050009"
#define LED_ON "060001"
#define LED_OFF "060000"

#define motor_steps_1000 "0203E8"	//1 circle
#define motor_steps_500 "0201F4"	//half circle

#define timer_interval 20*1000*1000  //20ms
#define t_set_goal 5						//100ms
#define t_rotate_goal 500				//10s			
#define t_delay_goal	150				//3s						
#define t_led_goal	150					//3s						
#define t_go_back_goal		50				//1s

#define initial_seq_goal 3
#define detect_face_seq_goal 4
#define go_back_seq_goal	5


struct send_command {
	bool send;
	char command[7];
};
enum timer_state{t_set=0, t_rotate, t_delay, t_led, t_go_back};

class motor_control {
	private:
		bool initial_ok;
		int initial_seq;
		bool detect_face_ok;
		int detect_face_seq;
		bool delay_ok;
		bool detect_eyes_ok;
		bool go_back_ok;
		int go_back_seq;
		bool all_action_finished;
		int timer_counter;
		bool timer_lock;
		bool MTR_change_direction;
		int timer_goal;
		bool already_set;
		unsigned short int all_action_seq;
		bool printfed;

		void set_MTR_change_DIR(bool state);
		bool get_MTR_change_DIR();
		void set_timer_goal(timer_state number);
		void set_already_set_flag(bool state);
		bool get_already_set_flag();
	public:
		motor_control();
		~motor_control();	
		struct send_command SC;

		//about initial
		void set_initial_flag(bool state);	
		bool get_initial_flag();
		unsigned short int  get_initial_seq();
		void clean_initial_seq();
		void add_initial_seq();
		
		//about face detecton	
		void set_detect_face_flag(bool state);
		bool get_detect_face_flag();
		void add_detect_face_seq();
		unsigned short int get_detect_face_seq();
		void clean_detect_face_seq();
	
		//about delay
		void set_delay_ok_flag(bool state);
		bool get_delay_ok_flag();

		//about eyes detection
		void set_detect_eyes_flag(bool state);
		bool get_detect_eyes_flag();
	
		//about go back 
		void set_go_back_flag(bool state);
		bool get_go_back_flag();
		void add_go_back_seq();
		unsigned short int get_go_back_seq();
		void clean_go_back_seq();

		//about timer counter
		void add_timer_counter();
		int get_timer_counter();
		void clean_timer_counter();
		int get_timer_goal();
		void set_timer_lock_flag(bool state);
		bool get_timer_lock_flag();
		
		//about all action finish
		bool get_all_action_finished_flag();
		void set_all_action_finished_flag(bool state);
		void set_all_action_seq(short int);
		unsigned short int get_all_action_seq();
	  void run_action(struct send_command* src);
		
		//about print run_action log
		void set_printfed_flag(bool state);
		bool get_printfed_flag();
};


#endif

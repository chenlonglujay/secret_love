#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H

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
#define LED_on "060001"
#define LED_off "060000"

#define motor_steps_1000 "0203E8"	//1 circle
#define motor_steps_500 "0201F4"	//half circle
#define run_count_set 1
#define run_count_rotate 150
#define timer_interval 100000
struct motor_initial_state{
		bool timer_unlock;
		bool finish;
		int seq;
		bool action_done;
};

struct detect_state{
	bool face_detect;
	bool eyes_detect;
	bool all_detect_ok;
};

struct motor_run_state{
	bool change_DIR;
	int seq;
	int timer_count;
	bool timer_unlock;
	bool finish;
	int count_goal;
};

void setup_motor_initial_state(struct motor_initial_state *st) {
	st->timer_unlock = false;
	st->finish = false;
	st->seq = 0;
	st->action_done = false;
}

void setup_detect_state(struct detect_state *st) {
	st->face_detect = false;
	st->eyes_detect = false;
	st->all_detect_ok = false;
}	

void setup_motor_run_initial(struct motor_run_state *st,bool is_finish) {
	st->change_DIR = false;
	st->seq = 0;
	st->timer_count = 0;
	st->timer_unlock = false;
	st->finish = is_finish;
	st->count_goal = run_count_set; 
}	
#endif

#include "motor_control.h"

motor_control::motor_control() {

	set_initial_flag(false);
	clean_initial_seq();
	set_detect_face_flag(false);
	clean_detect_face_seq();
	set_delay_ok_flag(false);
	set_detect_eyes_flag(false);
	set_go_back_flag(false);
	clean_go_back_seq();
	set_all_action_finished_flag(0);
	clean_timer_counter();
	set_timer_lock_flag(false);
	set_MTR_change_DIR(true);
	set_already_set_flag(false);
	set_timer_goal(t_set);
	set_all_action_seq(0);
	SC.send = false;
}

motor_control::~motor_control() {
	//nothing
}



void motor_control::set_initial_flag(bool state) {
	initial_ok = state;
}

bool motor_control::get_initial_flag() {
	return initial_ok;
}

void motor_control::add_initial_seq() {
	initial_seq++;
	set_already_set_flag(false);
}

unsigned short int motor_control::get_initial_seq() {
	return initial_seq;
}

void motor_control::clean_initial_seq() {
	initial_seq = 0;
}

void motor_control::set_detect_face_flag(bool state) {
	detect_face_ok = state;
}

bool motor_control::get_detect_face_flag() {
	return detect_face_ok;
}

unsigned short int motor_control::get_detect_face_seq() {
	return detect_face_seq;
}

void motor_control::add_detect_face_seq() {
	detect_face_seq++;
	set_already_set_flag(false);
}

void motor_control::clean_detect_face_seq() {
	detect_face_seq = 0;
}

void motor_control:: set_delay_ok_flag(bool state) {
	delay_ok = state;
}
		
bool motor_control::get_delay_ok_flag() {
	return delay_ok;
}


void motor_control::set_detect_eyes_flag(bool state) {
	detect_eyes_ok = state;
}
		
bool motor_control::get_detect_eyes_flag() {
	return detect_eyes_ok;
}

void motor_control::set_go_back_flag(bool state) {
	go_back_ok = state;
}
		
bool motor_control::get_go_back_flag() {
	return go_back_ok;
}
		
void motor_control:: add_go_back_seq() {
	go_back_seq++;
}

unsigned short int motor_control::get_go_back_seq() {
	return go_back_seq;
}

void motor_control::clean_go_back_seq() {
	go_back_seq = 0;
}

void motor_control::set_all_action_finished_flag(bool state) {
	all_action_finished = state;
}

bool motor_control::get_all_action_finished_flag() {
	return all_action_finished;
}

void motor_control::add_timer_counter() {
	timer_counter++;
}
		
int motor_control::get_timer_counter() {
	return timer_counter;;
}

void motor_control::clean_timer_counter() {
	timer_counter = 0;
	set_already_set_flag(false);
}

void motor_control::set_timer_lock_flag(bool state) {
	timer_lock = state;	
}

bool motor_control::get_timer_lock_flag() {
	return timer_lock;
}

void motor_control::set_MTR_change_DIR(bool state) {
	MTR_change_direction = state;
}

int motor_control::get_timer_goal() {
	return timer_goal;
}

bool motor_control::get_MTR_change_DIR() {
	return MTR_change_direction;
}

void motor_control::set_timer_goal(timer_state number) {
	switch(number) {
		case t_set:
			timer_goal = t_set_goal;
		break;
		case t_rotate:
			timer_goal = t_rotate_goal;
		break;
		case t_delay:
			timer_goal = t_delay_goal;
		break;
		case t_led:
			timer_goal = t_led_goal;
		break;
		case t_go_back:
			timer_goal = t_go_back_goal;
		break;
	}
}

void motor_control::set_printfed_flag(bool state) {
	printfed = state;
}

bool motor_control::get_printfed_flag() {
	return printfed;
}

void motor_control::run_action(struct send_command *src) {


	if(!get_initial_flag() && get_all_action_seq()==0) {
		src->send = true;
		switch(get_initial_seq()) {
			case 0:
				if(!get_already_set_flag()){
					set_already_set_flag(true);
					strcpy(src->command, motor_nothing);
					set_timer_goal(t_set);
					if(!get_printfed_flag()) {
						printf("motor nothing\n");
						set_printfed_flag(true);
					}
					set_timer_lock_flag(false);
				}
			break;
			case 1:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);
					strcpy(src->command, motor_spd6);
					set_timer_goal(t_set);
					if(!get_printfed_flag()) {
						printf("motor spd6\n");
						set_printfed_flag(true);
					}
					set_timer_lock_flag(false);
				}
			break;
			case 2:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);
					strcpy(src->command, LED_OFF);
					set_timer_goal(t_set);
					if(!get_printfed_flag()) {
						printf("LED OFF and initial ok\n");
						set_printfed_flag(true);
					}
					set_timer_lock_flag(false);
				}
			break;
		};
	} else if(!get_detect_face_flag() && get_all_action_seq()==1) {
		//motor rotating and starting to detect
		src->send = true;
		switch(get_detect_face_seq()) {
			case 0:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);
					if(get_MTR_change_DIR()) {
						strcpy(src->command, CW);
						set_MTR_change_DIR(false);
						if(!get_printfed_flag()) {
							printf("motor CW\n");
							set_printfed_flag(true);
						}
					} else if(!get_MTR_change_DIR()) {
						strcpy(src->command, CCW);
						set_MTR_change_DIR(true);
						if(!get_printfed_flag()) {
							printf("motor CCW\n");
							set_printfed_flag(true);
						}
					}
					set_timer_goal(t_set);
					set_timer_lock_flag(false);
				}
			break;
			case 1:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);

					strcpy(src->command, motor_spd6);
					set_timer_goal(t_set);
					if(!get_printfed_flag()) {
						printf("motor spd6\n");
						set_printfed_flag(true);
					}
					set_timer_lock_flag(false);
				}
			break;
			case 2:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);

					strcpy(src->command, motor_steps_1000);
					set_timer_goal(t_rotate);
					if(!get_printfed_flag()) {
						printf("motor run 1000 steps\n");
						set_printfed_flag(true);
					}
					set_timer_lock_flag(false);
				}
			break;
			case 3:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);

					strcpy(src->command, motor_stop);
					set_timer_goal(t_set);
					if(!get_printfed_flag()) {
						printf("motor run stop\n");
						printf("wait for face detection\n");
						set_printfed_flag(true);
					}
					set_timer_lock_flag(false);
				}
			break;
		};
	} else if(!get_delay_ok_flag() && get_all_action_seq()==2) {
		if(!get_already_set_flag()){
			set_already_set_flag(true);

			src->send = false;
			set_timer_goal(t_delay);
			if(!get_printfed_flag()) {
				printf("setting delay\n");
				set_printfed_flag(true);
			}
			set_timer_lock_flag(false);
		}
	} else if(!get_detect_eyes_flag() && get_all_action_seq()==3 ) {
		//after delay,it will be starting to detect eyes 
		if(!get_already_set_flag()){
			set_already_set_flag(true);
			src->send = false;
			if(!get_printfed_flag()) {
				printf("wait for eyes detection\n");
				set_printfed_flag(true);
			}
		}
	}	else if(!get_go_back_flag() && get_all_action_seq()==4) {
		//detected eyes ,LED ON and motor rotate opposite direction
		src->send = true;
		switch(get_go_back_seq()) {
			case 0:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);
					strcpy(src->command, CCW);
					if(!get_printfed_flag()) {
						printf("go_back:motor CCW\n");
					}
					set_timer_goal(t_set);
					set_timer_lock_flag(false);
				}
			break;
			case 1:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);
					strcpy(src->command, motor_spd3);
					set_timer_goal(t_set);
					if(!get_printfed_flag()) {
						printf("go_back:motor spd3\n");
					}
					set_timer_lock_flag(false);
				}
			break;
			case 2:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);
					strcpy(src->command, LED_ON);
					set_timer_goal(t_led);
					if(!get_printfed_flag()) {
						printf("go_back:LED ON\n");
					}
					set_timer_lock_flag(false);
				}
			break;
			case 3:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);
					strcpy(src->command, motor_steps_500);
					set_timer_goal(t_go_back);
					if(!get_printfed_flag()) {
						printf("go_back:motor run 500 steps\n");
					}
					set_timer_lock_flag(false);
				}
			break;
			case 4:
				if(!get_already_set_flag()) {
					set_already_set_flag(true);
					strcpy(src->command, motor_stop);
					set_timer_goal(t_set);
					if(!get_printfed_flag()) {
						printf("go_back:motor run stop\n");
					}
					set_timer_lock_flag(false);
				}
			break;
		};
	}	

}

void motor_control::set_all_action_seq(short int val) {
	all_action_seq = val;
}

unsigned short int motor_control::get_all_action_seq() {
	return all_action_seq;
}

void motor_control::set_already_set_flag(bool state) {
	already_set = state;
}
		     
bool motor_control::get_already_set_flag() {
	return already_set;
}


all:
	g++ `pkg-config --libs opencv` main.cpp ../send_I2C_command.cpp motor_control.cpp -lpthread -lrt -o main
clean:
	rm main


#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>


#define IN1_PIN 1
#define IN2_PIN 4
#define IN3_PIN 5
#define IN4_PIN 6
#define MAX_SPEED 55
#define MIN_SPEED 0
#define LEFT_TRACER_PIN 10
#define RIGHT_TRACER_PIN 11
#define MIDDLE_IR 27

#define TRIG_PIN	28
#define ECHO_PIN	29

int leftTracer, rightTracer;
int middleIrInput = 1;
int counter = 0;
int turning = 0;
int pedestrianFlag = 0;
int objectDetectedFlag = 0;

void initDCMotorSoft() {
	pinMode(IN1_PIN, SOFT_PWM_OUTPUT);
	pinMode(IN2_PIN, SOFT_PWM_OUTPUT);
	pinMode(IN3_PIN, SOFT_PWM_OUTPUT);
	pinMode(IN4_PIN, SOFT_PWM_OUTPUT);
	softPwmCreate(IN1_PIN, MIN_SPEED, MAX_SPEED);
	softPwmCreate(IN2_PIN, MIN_SPEED, MAX_SPEED);
	softPwmCreate(IN3_PIN, MIN_SPEED, MAX_SPEED);
	softPwmCreate(IN4_PIN, MIN_SPEED, MAX_SPEED);
}

void goForwardSoft() {
	softPwmWrite(IN1_PIN, MAX_SPEED);
	softPwmWrite(IN2_PIN, MIN_SPEED);
	softPwmWrite(IN3_PIN, MAX_SPEED);
	softPwmWrite(IN4_PIN, MIN_SPEED);
}

void goBackwardSoft() {
	softPwmWrite(IN1_PIN, MIN_SPEED);
	softPwmWrite(IN2_PIN, MAX_SPEED);
	softPwmWrite(IN3_PIN, MIN_SPEED);
	softPwmWrite(IN4_PIN, MAX_SPEED);
}

void stopDCMotorSoft() {
	softPwmWrite(IN1_PIN, MIN_SPEED);
	softPwmWrite(IN2_PIN, MIN_SPEED);
	softPwmWrite(IN3_PIN, MIN_SPEED);
	softPwmWrite(IN4_PIN, MIN_SPEED);
}


void turnLeftSoft() {
	softPwmWrite(IN1_PIN, MIN_SPEED);
	softPwmWrite(IN2_PIN, MAX_SPEED);
	softPwmWrite(IN3_PIN, MAX_SPEED);
	softPwmWrite(IN4_PIN, MIN_SPEED);
}

void turnRightSoft() {
	softPwmWrite(IN1_PIN, MAX_SPEED);
	softPwmWrite(IN2_PIN, MIN_SPEED);
	softPwmWrite(IN3_PIN, MIN_SPEED);
	softPwmWrite(IN4_PIN, MAX_SPEED);
}

void initDCMotorHard() {
	pinMode(IN1_PIN, OUTPUT);
	pinMode(IN2_PIN, OUTPUT);
	pinMode(IN3_PIN, OUTPUT);
	pinMode(IN4_PIN, OUTPUT);
	digitalWrite(IN1_PIN, HIGH);
	digitalWrite(IN2_PIN, HIGH);
	digitalWrite(IN3_PIN, HIGH);
	digitalWrite(IN4_PIN, HIGH);
}

void stopDCMotorHard() {
	digitalWrite(IN1_PIN, LOW);
	digitalWrite(IN2_PIN, LOW);
	digitalWrite(IN3_PIN, LOW);
	digitalWrite(IN4_PIN, LOW);
}

void turnLeftHard() {
	digitalWrite(IN1_PIN, 0);
	digitalWrite(IN2_PIN, 1);
	digitalWrite(IN3_PIN, 1);
	digitalWrite(IN4_PIN, 0);
	delay(335);
}

void turnRightHard() {
	digitalWrite(IN1_PIN, 1);
	digitalWrite(IN2_PIN, 0);
	digitalWrite(IN3_PIN, 0);
	digitalWrite(IN4_PIN, 1);
	delay(335);
}

void goForwardHard() {
	digitalWrite(IN1_PIN, 1);
	digitalWrite(IN2_PIN, 0);
	digitalWrite(IN3_PIN, 1);
	digitalWrite(IN4_PIN, 0);
}

void goBackwardHard() {
	digitalWrite(IN1_PIN, 0);
	digitalWrite(IN2_PIN, 1);
	digitalWrite(IN3_PIN, 0);
	digitalWrite(IN4_PIN, 1);
}

void initIR() {
	pinMode(LEFT_TRACER_PIN, INPUT);
	pinMode(RIGHT_TRACER_PIN, INPUT);
	pinMode(MIDDLE_IR, INPUT);
	pinMode(RIGHT_IR, INPUT);
}

int middleIrDetectFunction() {
	while(1) {
		delay(5);
		middleIrInput = digitalRead(MIDDLE_IR);
		if (middleIrInput == 0) {
			if (objectDetectedFlag == 1) {
				continue;
			}
			// for sparking, need delay
			delay(200);
			middleIrInput = digitalRead(MIDDLE_IR);
			if (middleIrInput == 0) {
				objectDetectedFlag = 1;
			} else {
				objectDetectedFlag = 0;
			}
		} else {
			if (objectDetectedFlag == 0) {
				continue;
			}
			// for sparking, need delay
			delay(200);
			middleIrInput = digitalRead(MIDDLE_IR);
			if(middleIrInput == 1) {
				objectDetectedFlag = 0;
			} else {
				objectDetectedFlag = 1;
			}
		}
	}
}


void fetchLineTracers() {
	leftTracer = digitalRead(LEFT_TRACER_PIN);
	rightTracer = digitalRead(RIGHT_TRACER_PIN);
}

void fullStop() {
	stopDCMotorHard();
	delay(10);
	stopDCMotorSoft();
	delay(10);
}

void passObjectFunction() {
	stopDCMotorSoft();
	delay(1500);
	initDCMotorHard();
	turnRightHard();
	stopDCMotorHard();
	goForwardHard();
	delay(100);
	stopDCMotorHard();
	delay(100);
	initDCMotorSoft();
	
	goForwardSoft();
	delay(5);
	while(1) {
		fetchLineTracers();
		if (leftTracer == 0 && rightTracer == 0) {
			stopDCMotorSoft();
			delay(100);
			break;
		}
	}
	goBackwardSoft();
	delay(200);
	stopDCMotorSoft();
	delay(1500);
	initDCMotorHard();

	turnLeftHard();
	stopDCMotorHard();
	delay(1500);
	initDCMotorSoft();
	goForwardSoft();
	delay(5);
	while(1) {
		fetchLineTracers();
		if (leftTracer == 1 && rightTracer == 0 ) {
			turnRightSoft();
			delay(25);
		} else if (leftTracer == 0 && rightTracer == 1) {
			turnLeftSoft();
			delay(25);
		}else if(leftTracer == 0 && rightTracer == 0) {
			stopDCMotorSoft();
			delay(100);
			break;
		} else {
			goForwardSoft();
		}
	}
	goBackwardSoft();
	delay(200);
	stopDCMotorSoft();
	delay(1500);
	initDCMotorHard();
	turnLeftHard();
	stopDCMotorHard();
	delay(500);
	goForwardHard();
	delay(350);
	stopDCMotorHard();
	delay(1500);
	turnRightHard();
	stopDCMotorHard();
	initDCMotorSoft();
}

void generalLineTracing() {
	if (counter == 0) {
		// Needed for starters
		goForwardSoft();
		delay(500);
		counter = 1;
	}
	if (leftTracer == 1 && rightTracer == 0 ) {
		turnRightSoft();
		delay(30);
	} else if (leftTracer == 0 && rightTracer == 1) {
		turnLeftSoft();
		delay(30);
	} else if(leftTracer == 0 && rightTracer == 0) {
		if(counter == 1) {
			// If pedestrian crosswalk will return, it will help!
			// DONT REMOVE
			stopDCMotorSoft();
			delay(1000);
			initDCMotorHard();
			goForwardHard();
			delay(700);
			stopDCMotorHard();
			initDCMotorSoft();
			counter = 2;
		} else if(counter == 2) {
			fullStop();
			break;
		}
	} else if(leftTracer == 1 && rightTracer == 1) {
		goForwardSoft();
		delay(5);
	}
}

void detectObjectParkInRed() {
	// Pedestrian passed
	// Korobka time
	passObjectFunction();
	goForwardSoft();
	delay(5);
	while(1) {
		fetchLineTracers();
		if(leftTracer == 0 && rightTracer == 0) {
			stopDCMotorSoft();
			delay(100);
			break;
		}
		if (leftTracer == 1 && rightTracer == 0 ) {
			turnRightSoft();
			delay(25);
		} else if (leftTracer == 0 && rightTracer == 1) {
			turnLeftSoft();
			delay(25);
		} else {
			goForwardSoft();
		}
	}
	goBackwardSoft();
	delay(200);
	stopDCMotorSoft();
	delay(1500);
	initDCMotorHard();
	turnLeftHard();
	stopDCMotorHard();
	delay(1500);
	goForwardHard();
	delay(700);
	stopDCMotorHard();
	delay(2000);
	// Parked in red zone;
}

void passObjectOnBackZone() {
	// Turn inside
	turnLeftHard();
	turnLeftHard();

	// Go back to object
	stopDCMotorHard();
	delay(1500);
	goForwardHard();
	delay(700);
	stopDCMotorHard();
	delay(1500);
	turnRightHard();
	stopDCMotorHard();
	delay(1500);
	stopDCMotorSoft();
	initDCMotorSoft();

	goForwardSoft();
	delay(5);
	while(1) {
		fetchLineTracers();
		if (objectDetectedFlag == 1) {
			// Korobka met again
			break;
		}
		if (leftTracer == 1 && rightTracer == 0 ) {
			turnRightSoft();
			delay(25);
		} else if (leftTracer == 0 && rightTracer == 1) {
			turnLeftSoft();
			delay(25);
		} else {
			goForwardSoft();
		}
	}
	stopDCMotorSoft();
	delay(100);
	// Pass object again
	passObjectFunction();
}

int main(void) {
	if(wiringPiSetup() == -1)
		return 0;

	pinMode(TRIG_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);

	initIR();

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, middleIrDetectFunction, NULL);

	initDCMotorSoft();
	while (1) {
		fetchLineTracers();
		if (objectDetectedFlag == 1) {
			if (pedestrianFlag == 1) {
				detectObjectParkInRed();
				passObjectOnBackZone();
				goForwardSoft();
				delay(5);
				counter = 0;
				while(1) {
					fetchLineTracers();
					if(leftTracer == 0 && rightTracer == 0) {
						fullStop();
						delay(200);
						break;
						return;
					}
					if (leftTracer == 1 && rightTracer == 0 ) {
						turnRightSoft();
						delay(25);
					} else if (leftTracer == 0 && rightTracer == 1) {
						turnLeftSoft();
						delay(25);
					} else {
						goForwardSoft();
					}
				}
			} else {
				// For pedestiran
				while(objectDetectedFlag == 1) {
					stopDCMotorSoft();
					delay(100);
				}
				pedestrianFlag = 1;
				continue;
			}
		}

		generalLineTracing();
	}
}



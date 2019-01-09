#include <wiringPi.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

long startTime = 0;

long getUS() {
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

	return 1000000 * spec.tv_sec + round(spec.tv_nsec / 1.0e3);
}

void sendTrigger() {
	digitalWrite(4, HIGH);
	startTime = getUS();
	long waitTime = getUS();
	while (getUS() - waitTime < 10) {};
	digitalWrite(4, LOW);
	return;
}

int main (void) {
	printf("HELLO");
	wiringPiSetup();
	pinMode(4, OUTPUT); // TIRGGER LINE
	digitalWrite(4, LOW);
	pinMode(5, INPUT); // ECHO LINE
	
	printf(" WORLD\n");
	
	enum State {
		init,
		waitingForEcho,
		waitToRestart,
	} state; 
	
	for(;;){
		// START OF INIFITE FOR LOOP
		switch(state) {
			case init:
				printf("INIT\n");
				digitalWrite(4, LOW);
				delay (60);
				sendTrigger();
				state = waitingForEcho;
			break;
			
			case waitingForEcho:
				if (digitalRead(5) == 1) {
					long echoTime = getUS();
					float distanceCM = (float)(echoTime - startTime) / 58.0;
					printf("Distance: %f\n", distanceCM);
					state = waitToRestart;
				}
				
				if (getUS() - startTime > 1000000) {
					printf("Didn't see an echo. retrying.\n");
					sendTrigger();
				} 
			break;
			
			case waitToRestart:
				if (startTime - getUS() > 60000) {
					sendTrigger();
					state = waitingForEcho;
				}
			break;
		}		
	}
	
	return 0;
}

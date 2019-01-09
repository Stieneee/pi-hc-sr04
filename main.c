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
	while (getUS() - waitTime < 15) {};
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
	
	long echoStartTime;
	
	enum State {
		init,
		waitingForEcho,
		measurePulse,
		waitToRestart,
	} state; 
	
	for(;;){
		// START OF INIFITE FOR LOOP
		switch(state) {
			case init:
				// This is only executed once
				printf("INIT\n");
				digitalWrite(4, LOW);
				delay (60);
				sendTrigger();
				state = waitingForEcho;
			break;
			
			case waitingForEcho:
				// If the ECHO is HIGH
				if (digitalRead(5) == 1) {
					// Pulse seen! Begin measuring pulse.
					echoStartTime = getUS();
					state = measurePulse;
				}
				
				// If we never saw a pulse :(
				if (getUS() - startTime > 1000000) {
					printf("Didn't see an echo. retrying.\n");
					sendTrigger();
				} 
			break;
			
			case measurePulse:
				// If the ECHO is LOW
				if (digitalRead(5) == 0) {
					// Pulse finished. Calc distance
					float distanceCM = ((float)getUS() - echoStartTime) / 58;
					printf("Distance: %f cm\n", distanceCM);
					state = waitToRestart;
				}
			break;
			
			case waitToRestart:
				// If we have waited long enough
				if (getUS() - startTime > 120000) {
					// Time to restart
					sendTrigger();
					state = waitingForEcho;
				}
			break;
		}		
	}
	
	return 0;
}

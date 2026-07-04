/*
* Advanced Countimer example.
*
* A count-up timer controlled over the serial monitor. Demonstrates:
*  - pause/resume (time spent paused is NOT counted),
*  - state queries: isCounterRunning(), isStopped(), isCounterCompleted(),
*  - reading the time as separate H/M/S components,
*  - re-programming the count time with the 3-argument setCounter(),
*  - calibrating the hardware millis() drift with setCalibration().
*
* Open the serial monitor, press one of the keys below and click 'Send':
* 'S' - start / resume the timer
* 'P' - pause the timer
* 'R' - restart the timer from the beginning
* 'T' - stop the timer
* '1' - re-program the count time to 15 seconds
* '?' - print the current timer state
*/
#include <Countimer.h>

Countimer timer;

void setup()
{
	Serial.begin(9600);

	// Count up from 00h:00m:00s to 00h:00m:30s, then call onComplete() once.
	timer.setCounter(0, 0, 30, timer.COUNT_UP, onComplete);

	// Call printTime() every 1000 ms while the timer is running.
	timer.setInterval(printTime, 1000);

	// Optional one-time, per-board correction of the hardware clock drift.
	// Run a long count against a reference clock, then set:
	// factor = real_elapsed_seconds / timer_indicated_seconds.
	// 1.0 (the default) means no correction.
	timer.setCalibration(1.0);

	Serial.println("Press one of the keys below and click 'Send':");
	Serial.println("'S' - start / resume the timer");
	Serial.println("'P' - pause the timer");
	Serial.println("'R' - restart the timer from the beginning");
	Serial.println("'T' - stop the timer");
	Serial.println("'1' - re-program the count time to 15 seconds");
	Serial.println("'?' - print the current timer state");
}

void loop()
{
	// run() is the heartbeat - it must be called on every loop() iteration.
	timer.run();

	if (Serial.available() > 0)
	{
		char c = toupper(Serial.read());

		switch (c)
		{
			case 'S':
				// start() also resumes after pause() - time spent paused
				// is not counted, so the count stays accurate.
				timer.start();
				Serial.println("Started.");
				break;
			case 'P':
				timer.pause();
				Serial.println("Paused - press 'S' to resume.");
				break;
			case 'R':
				timer.restart();
				Serial.println("Restarted.");
				break;
			case 'T':
				timer.stop();
				Serial.println("Stopped.");
				break;
			case '1':
				// Re-program the count time of the existing timer;
				// the mode and completion callback are kept.
				timer.setCounter(0, 0, 15);
				Serial.println("Count time set to 15 seconds.");
				break;
			case '?':
				printState();
				break;
			default:
				break;
		}
	}
}

void printTime()
{
	// The time can be read as separate components instead of the
	// pre-formatted getCurrentTime() string.
	Serial.print("Elapsed: ");
	Serial.print(timer.getCurrentHours());
	Serial.print("h ");
	Serial.print(timer.getCurrentMinutes());
	Serial.print("m ");
	Serial.print(timer.getCurrentSeconds());
	Serial.println("s");
}

void printState()
{
	Serial.print("running=");
	Serial.print(timer.isCounterRunning());
	Serial.print(" stopped=");
	Serial.print(timer.isStopped());
	Serial.print(" completed=");
	Serial.println(timer.isCounterCompleted());
}

void onComplete()
{
	Serial.println("Complete! Press 'R' to run again.");
}

/*
* Basic Countimer example.
*
* Counts down from 10 seconds, prints the remaining time on the serial
* monitor every second and reports when the count is complete.
*/
#include <Countimer.h>

Countimer timer;

void setup()
{
	Serial.begin(9600);

	// Count down from 00h:00m:10s and call onComplete() when finished.
	timer.setCounter(0, 0, 10, timer.COUNT_DOWN, onComplete);

	// Call printTime() every 1000 ms while the timer is running.
	timer.setInterval(printTime, 1000);

	// Nothing happens until the timer is started.
	timer.start();
}

void loop()
{
	// run() is the heartbeat - it must be called on every loop() iteration.
	timer.run();
}

void printTime()
{
	Serial.print("Remaining time: ");
	Serial.println(timer.getCurrentTime());
}

void onComplete()
{
	Serial.println("Complete!");
}

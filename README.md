# Countimer

This is simple timer and counter Arduino library.  
Offers three work modes:

 * Count-up timer with call specified method when count is complete.
 * Count-down timer with call specified method when count is complete.
 * Calling any method at a specified time interval.


It allows you to start/pause, stop or restart timer.  
The following are public methods for actions:

 * void start()
 * void stop()
 * void pause()
 * void restart()


 Other methods:

 * byte getCurrentHours()
 * byte getCurrentMinutes()
 * byte getCurrentSeconds()
 * void setInterval()
 * String getCurrentTime()
 * bool isCounterCompleted()
 * bool isCounterRunning()
 * bool isStopped()



And here's some sample code!

```c
#include "Countimer.h"

Countimer timer;

void setup() {
	Serial.begin(9600);

    // Set up count down timer with 10s and call method onComplete() when timer is complete.
    // 00h:00m:10s
	timer.setCounter(0, 0, 10, timer.COUNT_DOWN, onComplete);

    // Print current time every 1s on serial port by calling method refreshClock().
    timer.setInterval(refreshClock, 1000);
}

void refreshClock() {
	Serial.print("Current count time is: ");
    Serial.println(timer.getCurrentTime());
}

void onComplete() {
	Serial.println("Complete!!!");
}

void loop() {
	// Run timer
	timer.run();

    // Now timer is running and listening for actions.
    // If you want to start the timer, you have to call start() method.
    if(!timer.isCounterCompleted()) {
      timer.start();
    }
}
```

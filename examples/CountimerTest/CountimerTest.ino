/*
* Open serial monitor, press one of the keys below and click 'Send':
* 'S' - to start all timers
* 'P' - to pause all timers
* 'R' - to restart all timers
* 'T' - to stop all timers
*/
#include "Countimer.h"

Countimer tUp;
Countimer tDown;
Countimer tNone;

void setup()
{
	Serial.begin(9600);
        
    // Count-up timer with 10s
	tUp.setCounter(0, 0, 10, tUp.COUNT_UP, tUpComplete);
    // Call print_time1() method every 1s.
	tUp.setInterval(print_time1, 1000);

    // Count-down timer with 21s
	tDown.setCounter(0, 0, 21, tDown.COUNT_DOWN, tDownComplete);
    // Call print_time2() method every 1s.
	tDown.setInterval(print_time2, 1000);

    // No counter
    // Just call print_none() method every 2s.
	tNone.setInterval(print_none, 2000);

	Serial.println("Press one of the keys below and click 'Send':");
	Serial.println("'S' - to start all timers");
	Serial.println("'P' - to pause all timers");
	Serial.println("'R' - to restart all timers");
	Serial.println("'T' - to stop all timers");
}

void loop()
{
	tUp.run();
	tDown.run();
	tNone.run();

	if (Serial.available() > 0)
	{
		char c = toupper(Serial.read());

		switch (c)
		{
			case 'T':
				tUp.stop();
				tDown.stop();
				tNone.stop();
				break;
			case 'R':
				tUp.restart();
				tDown.restart();
				tNone.restart();
				break;
			case 'S':
				tUp.start();
				tDown.start();
				tNone.start();
				break;
			case 'P':
				tUp.pause();
				tDown.pause();
				tNone.pause();
				break;
			default:
				break;
		}
	}
}

void print_time1()
{
	Serial.print("tUp: ");
	Serial.println(tUp.getCurrentTime());
}

void print_time2()
{
	Serial.print("tDown: ");
	Serial.println(tDown.getCurrentTime());
}

void print_none()
{
	Serial.print("tNone: millis(): ");
	Serial.println(millis());
}

void tUpComplete()
{
	digitalWrite(13, HIGH);
}

void tDownComplete()
{
	digitalWrite(13, LOW);
}
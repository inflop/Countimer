/*
* Open serial monitor, press one of the keys below and click 'Send':
* 'S' - to start all timers
* 'P' - to pause all timers
* 'R' - to restart all timers
* 'T' - to stop all timers
*/
#include <Countimer.h>

// Generic ESP32 boards (esp32:esp32:esp32 FQBN) don't define LED_BUILTIN in the core,
// unlike AVR boards where it's always available. Fall back to GPIO2, the onboard LED
// pin on most ESP32 dev boards, so this sketch compiles on both.
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

Countimer tUp;
Countimer tDown;
Countimer tNone;
Countimer tSubSecond;

void setup()
{
	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);


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

    // Count-down timer with sub-second precision: 1.5s
	tSubSecond.setCounter(0, 0, 1, 500, tSubSecond.COUNT_DOWN, tSubSecondComplete);
    // Call print_time_sub_second() method every 100ms.
	tSubSecond.setInterval(print_time_sub_second, 100);

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
	tSubSecond.run();

	if (Serial.available() > 0)
	{
		char c = toupper(Serial.read());

		switch (c)
		{
			case 'T':
				tUp.stop();
				tDown.stop();
				tNone.stop();
				tSubSecond.stop();
				break;
			case 'R':
				tUp.restart();
				tDown.restart();
				tNone.restart();
				tSubSecond.restart();
				break;
			case 'S':
				tUp.start();
				tDown.start();
				tNone.start();
				tSubSecond.start();
				break;
			case 'P':
				tUp.pause();
				tDown.pause();
				tNone.pause();
				tSubSecond.pause();
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

void print_time_sub_second()
{
	Serial.print("tSubSecond: ");
	Serial.println(tSubSecond.getCurrentTimeWithMillis());
}

void tUpComplete()
{
	digitalWrite(LED_BUILTIN, HIGH);
}

void tDownComplete()
{
	digitalWrite(LED_BUILTIN, LOW);
}

void tSubSecondComplete()
{
	Serial.println("tSubSecond: complete!");
}
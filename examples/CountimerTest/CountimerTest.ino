#include "Countimer.h"

Countimer tUp;
Countimer tDown;
Countimer tNone;

void setup()
{
	Serial.begin(9600);

	tUp.setCounter(0, 0, 10, tUp.COUNT_UP, tUpComplete);
	tUp.setInterval(print_time1, 1000);

	tDown.setCounter(0, 0, 21, tDown.COUNT_DOWN, tDownComplete);
	tDown.setInterval(print_time2, 1000);

	tNone.setInterval(print_none, 2000);
}

void loop()
{
	tUp.run();
	tUp.start();

	tDown.run();
	tDown.start();

	tNone.run();
	tNone.start();

	//Countimer::delay(10000);

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

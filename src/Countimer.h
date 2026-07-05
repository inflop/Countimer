#ifndef COUNTIMER_H
#define COUNTIMER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define COUNTIMER_MAX_HOURS 999
#define COUNTIMER_MAX_MINUTES_SECONDS 59
#define COUNTIMER_MAX_MILLISECONDS 999

typedef void(*timer_callback)(void);

class Countimer
{
public:
	enum CountType
	{
		COUNT_NONE = 0,
		COUNT_UP = 1,
		COUNT_DOWN = 2
	};

	// Set up counter time(hours, minutes, seconds), count mode and function to execute if count is completed.
	void setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds, CountType countType, timer_callback onComplete);

	// Set up counter time(hours, minutes, seconds, milliseconds), count mode and function to execute if count is completed.
	void setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, CountType countType, timer_callback onComplete);

	// Set up counter time(hours, minutes, seconds) for existing timer.
	void setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds);

	// Set up counter time(hours, minutes, seconds, milliseconds) for existing timer.
	void setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds);

	// Returns timer's current hours.
	uint16_t getCurrentHours() const;

	// Returns timer's current minutes.
	uint8_t getCurrentMinutes() const;

	// Returns timer's current seconds.
	uint8_t getCurrentSeconds() const;

	// Returns timer's current milliseconds (0-999).
	uint16_t getCurrentMilliseconds() const;

	void setInterval(timer_callback callback, uint32_t interval);

	// Set correction factor for hardware millis() drift, e.g. from ceramic resonator tolerance.
	// Default is 1.0 (no correction). Measure once per board: factor = real_elapsed_time / timer_indicated_time.
	void setCalibration(float factor);

	// Returns current timer as formatted string HH:MM:SS
	char* getCurrentTime();

	// Returns current timer as formatted string HH:MM:SS.mmm
	char* getCurrentTimeWithMillis();

	// Returns true if counter is completed, otherwise returns false.
	bool isCounterCompleted() const;

	// Returns true if counter is still running, otherwise returns false.
	bool isCounterRunning() const;

	// Returns true if timer is stopped, otherwise returns false.
	bool isStopped() const;

	// Run timer. This is main method.
	// If you want to start timer after run, you have to invoke start() method.
	void run();

	// Starting timer.
	void start();

	// Stopping timer.
	void stop();

	// Pausing timer.
	void pause();

	// Restart timer.
	void restart();

private:
	// Counting down timer by measured elapsed milliseconds.
	void countDown(uint32_t elapsed);

	void callback();
	void complete();

	// Counting up timer by measured elapsed milliseconds.
	void countUp(uint32_t elapsed);

	uint32_t _interval = 1;
	uint32_t _previousMillis = 0;

	// Correction factor for hardware millis() drift (1.0 = no correction).
	float _calibration = 1.0;

	// Carries fractional milliseconds between ticks so calibration rounding error does not accumulate.
	float _calibrationRemainder = 0.0;

	// Stores current counter value in milliseconds.
	uint32_t _currentCountTime = 0;
	uint32_t _startCountTime = 0;

	// Stores cached user's time.
	uint32_t _countTime = 0;

	// Function to execute.
	timer_callback _callback = nullptr;

	// Function to execute when timer is complete.
	timer_callback _onComplete = nullptr;
	bool _isCounterCompleted = false;
	bool _isStopped = true;
	char _formatted_time[10];
	char _formatted_time_ms[13];
	CountType _countType = COUNT_NONE;
};

#endif

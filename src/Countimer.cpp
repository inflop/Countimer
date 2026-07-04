#include "Countimer.h"

void Countimer::setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds, CountType countType, timer_callback onComplete)
{
	_onComplete = onComplete;
	_countType = countType;
	setCounter(hours, minutes, seconds);
}

void Countimer::setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds)
{
	if (hours > COUNTIMER_MAX_HOURS) {
		hours = COUNTIMER_MAX_HOURS;
	}

	if (minutes > COUNTIMER_MAX_MINUTES_SECONDS) {
		minutes = COUNTIMER_MAX_MINUTES_SECONDS;
	}

	if (seconds > COUNTIMER_MAX_MINUTES_SECONDS) {
		seconds = COUNTIMER_MAX_MINUTES_SECONDS;
	}

	_currentCountTime = ((hours * 3600L) + (minutes * 60L) + seconds) * 1000L;
	_countTime = _currentCountTime;

	if (_countType == COUNT_UP)
	{
		// if is count up mode, we have to start from 00:00:00;
		_currentCountTime = 0;
	}

	_startCountTime = _currentCountTime;

}

void Countimer::setInterval(timer_callback callback, uint32_t interval)
{
	_interval = interval;
	_callback = callback;
}

void Countimer::setCalibration(float factor)
{
	_calibration = (factor > 0) ? factor : 1.0;
}

uint16_t Countimer::getCurrentHours() const
{
	return _currentCountTime / 1000 / 3600;
}

uint8_t Countimer::getCurrentMinutes() const
{
	return _currentCountTime / 1000 % 3600 / 60;
}

uint8_t Countimer::getCurrentSeconds() const
{
	return _currentCountTime / 1000 % 60;
}

char* Countimer::getCurrentTime()
{
	snprintf(_formatted_time, sizeof(_formatted_time), "%02u:%02u:%02u",
		(unsigned int)getCurrentHours(), (unsigned int)getCurrentMinutes(), (unsigned int)getCurrentSeconds());
	return _formatted_time;
}

bool Countimer::isCounterCompleted() const
{
	return _isCounterCompleted;
}

bool Countimer::isCounterRunning() const
{
	return !_isStopped && !_isCounterCompleted;
}

bool Countimer::isStopped() const
{
	return _isStopped;
}

bool Countimer::isCounterRunning()
{
	return !_isStopped;
}

void Countimer::start()
{
	if(_isStopped)
	{
		// Reset the reference point only on a real resume, so paused time
		// is not counted. start() may be called on every loop() iteration.
		_isStopped = false;
		_previousMillis = millis();
		_calibrationRemainder = 0.0;
	}

	if(_isCounterCompleted)
	{
		_isCounterCompleted = false;
	}
}

void Countimer::pause()
{
	_isStopped = true;
}

void Countimer::stop()
{
	_isStopped = true;
	_isCounterCompleted = true;
	_currentCountTime = _countTime;

	if(_countType == COUNT_UP)
	{
		_currentCountTime = 0;
	}
}

void Countimer::restart()
{
	_currentCountTime = _startCountTime;
	_isCounterCompleted = false;

	// Force start() to reset the time reference point,
	// even when restarting a running timer.
	_isStopped = true;

	start();
}

void Countimer::run()
{
	// timer is running only if is not completed or not stopped.
	if (_isCounterCompleted || _isStopped)
		return;

	uint32_t now = millis();
	// Unsigned subtraction is safe across millis() overflow (~49 days).
	uint32_t elapsed = now - _previousMillis;

	if (elapsed >= _interval) {

		// Move the reference point before running callbacks, so neither the
		// check overshoot nor the callback execution time is ever lost.
		_previousMillis = now;

		// Apply hardware drift correction, carrying the fractional
		// remainder so rounding error does not accumulate over ticks.
		float corrected = elapsed * _calibration + _calibrationRemainder;
		uint32_t delta = (uint32_t)corrected;
		_calibrationRemainder = corrected - delta;

		if (_countType == COUNT_DOWN)
		{
			countDown(delta);
		}
		else if (_countType == COUNT_UP)
		{
			countUp(delta);
		}
		else
		{
			callback();
		}
	}
}

void Countimer::countDown(uint32_t elapsed)
{
	if (_currentCountTime > elapsed)
	{
		// Subtract the real measured time, not the nominal interval,
		// so the error does not accumulate over long countdowns.
		_currentCountTime -= elapsed;
		callback();
	}
	else
	{
		stop();
		complete();
	}
}

void Countimer::countUp(uint32_t elapsed)
{
	if (_currentCountTime + elapsed < _countTime)
	{
		// Add the real measured time, not the nominal interval,
		// so the error does not accumulate over long counts.
		_currentCountTime += elapsed;
		callback();
	}
	else
	{
		stop();
		complete();
	}
}

void Countimer::callback()
{
	if(_callback != nullptr)
		_callback();
}

void Countimer::complete()
{
	if(_onComplete != nullptr)
		_onComplete();
}

#include "Countimer.h"
#include "Arduino.h"

Countimer::Countimer()
{
	_previousMillis = 0;
	_currentCountTime = 0;
	_countTime = 0;
	_isCounterCompleted = false;
	_isStopped = true;
	_countType = COUNT_NONE;
	_startCountTime = 0;
}

Countimer::~Countimer()
{
}

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

uint16_t Countimer::getCurrentHours()
{
	return _currentCountTime / 1000 / 3600;
}

uint8_t Countimer::getCurrentMinutes()
{
	return _currentCountTime / 1000 % 3600 / 60;
}

uint8_t Countimer::getCurrentSeconds()
{
	return _currentCountTime / 1000 % 3600 % 60 % 60;
}

char* Countimer::getCurrentTime()
{
	sprintf(_formatted_time, "%02d:%02d:%02d", getCurrentHours(), getCurrentMinutes(), getCurrentSeconds());
	return _formatted_time;
}

bool Countimer::isCounterCompleted()
{
	return _isCounterCompleted;
}

bool Countimer::isStopped()
{
	return _isStopped;
}

void Countimer::start()
{
	_isStopped = false;
	if(_isCounterCompleted)
		_isCounterCompleted = false;
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
	_isStopped = false;

	start();
}

void Countimer::run()
{
	// timer is running only if is not completed or not stopped.
	if (_isCounterCompleted || _isStopped)
		return;

	if (millis() - _previousMillis >= _interval) {

		if (_countType == COUNT_DOWN)
		{
			countDown();
		}
		else if (_countType == COUNT_UP)
		{
			countUp();
		}
		else
		{
			callback();
		}
		_previousMillis = millis();
	}
}

void Countimer::countDown()
{
	if (_currentCountTime > 0)
	{
		callback();
		_currentCountTime -= _interval;
	}
	else
	{
		stop();
		complete();
	}
}

void Countimer::countUp()
{
	if (_currentCountTime < _countTime)
	{
		callback();
		_currentCountTime += _interval;
	}
	else
	{
		stop();
		complete();
	}
}

void Countimer::callback()
{
	if(_callback != NULL)
		_callback();
}

void Countimer::complete()
{
	if(_onComplete != NULL)
		_onComplete();
}

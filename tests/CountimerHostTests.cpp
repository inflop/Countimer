#include <cstdio>

#include "Countimer.h"

uint32_t g_fakeMillis = 0;

static int g_tickCalls = 0;
static int g_completeCalls = 0;
static int g_failures = 0;

static void resetCounters()
{
	g_tickCalls = 0;
	g_completeCalls = 0;
	g_failures = 0;
	g_fakeMillis = 0;
}

static void tickCallback()
{
	++g_tickCalls;
}

static void completeCallback()
{
	++g_completeCalls;
}

static void reportFailure(const char* file, int line, const char* expression)
{
	std::printf("%s:%d: check failed: %s\n", file, line, expression);
	++g_failures;
}

#define CHECK(expr) do { if (!(expr)) { reportFailure(__FILE__, __LINE__, #expr); } } while (false)

// COUNT_DOWN: verifies that the timer counts down correctly each tick,
// fires the tick callback on each interval, and calls the completion
// callback exactly once when the counter reaches zero.
static bool testCountDownCompletes()
{
	resetCounters();

	Countimer timer;
	timer.setCounter(0, 0, 2, Countimer::COUNT_DOWN, completeCallback);
	timer.setInterval(tickCallback, 1000);
	timer.start();

	g_fakeMillis = 1000;
	timer.run();
	CHECK(timer.isCounterRunning());
	CHECK(!timer.isCounterCompleted());
	CHECK(timer.getCurrentSeconds() == 1);
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 0);

	g_fakeMillis = 2000;
	timer.run();
	CHECK(timer.isCounterCompleted());
	CHECK(timer.isStopped());
	CHECK(timer.getCurrentSeconds() == 2);
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 1);

	return g_failures == 0;
}

// COUNT_UP: verifies that the timer starts from zero, increments on each tick,
// fires the tick callback on each interval, and calls the completion callback
// exactly once when the counter reaches the configured end time.
static bool testCountUpCompletes()
{
	resetCounters();

	Countimer timer;
	timer.setCounter(0, 0, 2, Countimer::COUNT_UP, completeCallback);
	timer.setInterval(tickCallback, 1000);
	timer.start();

	g_fakeMillis = 1000;
	timer.run();
	CHECK(timer.isCounterRunning());
	CHECK(timer.getCurrentSeconds() == 1);
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 0);

	g_fakeMillis = 2000;
	timer.run();
	CHECK(timer.isCounterCompleted());
	CHECK(timer.isStopped());
	CHECK(timer.getCurrentSeconds() == 0);
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 1);

	return g_failures == 0;
}

// pause/resume: verifies that pause() freezes the counter and that the time
// spent paused is not counted. The counter must continue from the same value
// after resume (start()), and the tick callback must not fire while paused.
static bool testPauseAndResume()
{
	resetCounters();

	Countimer timer;
	timer.setCounter(0, 0, 5, Countimer::COUNT_DOWN, completeCallback);
	timer.setInterval(tickCallback, 1000);
	timer.start();

	g_fakeMillis = 1000;
	timer.run();
	CHECK(timer.getCurrentSeconds() == 4);

	timer.pause();
	g_fakeMillis = 5000;
	timer.run();
	CHECK(timer.getCurrentSeconds() == 4);
	CHECK(g_tickCalls == 1);

	timer.start();
	g_fakeMillis = 6000;
	timer.run();
	CHECK(timer.getCurrentSeconds() == 3);
	CHECK(g_tickCalls == 2);

	return g_failures == 0;
}

// COUNT_NONE / interval-only mode: verifies that when only setInterval() is
// called (no setCounter()), the timer fires the tick callback every interval
// and never completes (isCounterRunning() stays true).
static bool testIntervalOnlyMode()
{
	resetCounters();

	Countimer timer;
	timer.setInterval(tickCallback, 250);
	timer.start();

	g_fakeMillis = 250;
	timer.run();
	CHECK(timer.isCounterRunning());
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 0);

	return g_failures == 0;
}

// setCounter clamping: verifies that values exceeding the allowed maxima
// (COUNTIMER_MAX_HOURS, COUNTIMER_MAX_MINUTES_SECONDS, COUNTIMER_MAX_MILLISECONDS)
// are silently clamped to those limits and not accepted as-is.
static bool testClampsConfiguredTime()
{
	resetCounters();

	Countimer timer;
	timer.setCounter(1200, 80, 90, 2000, Countimer::COUNT_DOWN, completeCallback);
	CHECK(timer.getCurrentHours() == 999);
	CHECK(timer.getCurrentMinutes() == 59);
	CHECK(timer.getCurrentSeconds() == 59);
	CHECK(timer.getCurrentMilliseconds() == 999);

	return g_failures == 0;
}

// millis() wraparound: verifies that the timer handles the uint32_t wraparound
// of millis() (~49-day overflow) correctly. Unsigned subtraction is inherently
// safe, so the elapsed time must be calculated correctly across the rollover.
static bool testMillisWraparound()
{
	resetCounters();

	Countimer timer;
	timer.setCounter(0, 0, 1, Countimer::COUNT_DOWN, completeCallback);
	timer.setInterval(tickCallback, 10);

	g_fakeMillis = 0xFFFFFFF0u;
	timer.start();

	g_fakeMillis = 0x00000020u;
	timer.run();
	CHECK(!timer.isCounterCompleted());
	CHECK(timer.getCurrentMilliseconds() == 952);
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 0);

	return g_failures == 0;
}

// calibration: verifies that setCalibration() scales the elapsed time by the
// given factor. A factor of 2.0 means each real millisecond counts as two,
// so the counter must advance twice as fast as without calibration.
static bool testCalibrationFactor()
{
	resetCounters();

	Countimer timer;
	timer.setCounter(0, 0, 3, Countimer::COUNT_DOWN, completeCallback);
	timer.setInterval(tickCallback, 1000);
	timer.setCalibration(2.0f);
	timer.start();

	g_fakeMillis = 1000;
	timer.run();
	CHECK(timer.getCurrentSeconds() == 1);
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 0);

	return g_failures == 0;
}

// COUNT_UP overflow regression: verifies that the completion check
// '_countTime - _currentCountTime > elapsed' does not overflow when elapsed
// is large enough that the old '_currentCountTime + elapsed < _countTime'
// expression would have wrapped around, causing a missed completion.
static bool testCountUpOverflowDoesNotWrap()
{
	resetCounters();

	Countimer timer;
	timer.setCounter(999, 59, 59, 999, Countimer::COUNT_UP, completeCallback);
	timer.setInterval(tickCallback, 1);
	timer.start();

	g_fakeMillis = 3500000000UL;
	timer.run();
	CHECK(timer.isCounterRunning());
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 0);

	g_fakeMillis = 1000000000UL;
	timer.run();
	CHECK(timer.isCounterCompleted());
	CHECK(timer.isStopped());
	CHECK(g_tickCalls == 1);
	CHECK(g_completeCalls == 1);
	CHECK(timer.getCurrentSeconds() == 0);

	return g_failures == 0;
}

typedef bool (*TestFn)();

struct TestCase
{
	const char* name;
	TestFn fn;
};

int main()
{
	const TestCase tests[] = {
		{ "count-down completes", testCountDownCompletes },
		{ "count-up completes", testCountUpCompletes },
		{ "pause and resume", testPauseAndResume },
		{ "interval-only mode", testIntervalOnlyMode },
		{ "configured time clamps", testClampsConfiguredTime },
		{ "millis wraparound", testMillisWraparound },
		{ "calibration factor", testCalibrationFactor },
		{ "count-up overflow", testCountUpOverflowDoesNotWrap },
	};

	int failedTests = 0;

	for (const TestCase& test : tests)
	{
		if (test.fn())
		{
			std::printf("[PASS] %s\n", test.name);
		}
		else
		{
			std::printf("[FAIL] %s\n", test.name);
			++failedTests;
		}
	}

	if (failedTests != 0)
	{
		std::fprintf(stderr, "%d test(s) failed\n", failedTests);
		return 1;
	}

	std::printf("All host tests passed\n");
	return 0;
}
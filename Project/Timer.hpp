#pragma once
#include <ctime>
#include <string>

class Timer
{
	time_t startTime;
	time_t stopTime;

public:
	Timer() :startTime(clock()), stopTime(0){}

	void Stop()
	{
		stopTime = clock();
	}

	double GetTime() const
	{
		return double(stopTime - startTime) / CLOCKS_PER_SEC;
	}

	string GetStringTime() const
	{
		return "\ntime=" + to_string(double(stopTime - startTime) / CLOCKS_PER_SEC);
	}
};

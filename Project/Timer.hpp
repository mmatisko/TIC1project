#pragma once
#include <ctime>
#include <string>
#include <vector>

class Timer
{
	time_t startTime;
	time_t stopTime;
	vector<time_t> lapTime;

public:
	Timer() :startTime(clock()), stopTime(0){}

	void Stop()
	{
		LapStop();
		stopTime = clock();
	}

	void LapStop()
	{
		lapTime.push_back(clock());
	}

	double GetTime() const
	{
		return double(stopTime - startTime) / CLOCKS_PER_SEC;
	}

	string GetStringTime() const
	{
		return "\ntime=" + to_string(double(stopTime - startTime) / CLOCKS_PER_SEC);
	}

	string GetLapsTime() const
	{
		string temp = "";
		time_t start = startTime;
		for (vector<time_t>::size_type i = 0; i != lapTime.size(); ++i) {
			temp += "lap " + to_string(i+1) + ": "
				+ to_string(double((lapTime[i] - start) / CLOCKS_PER_SEC)) + "\n";
			start = lapTime[i];
		}
		return temp;
	}
};

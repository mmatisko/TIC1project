#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

#include "sha256.h"
#include "bloom_filter.hpp"
#include "ThreadPool.h"

#define MULTITHREAD
#ifdef MULTITHREAD
#define THREAD_NUM 4
#endif
#define PREDICTED_SIZE 10000000

using namespace std;

ofstream logStringFile;
uint_fast64_t *base, result, filter_io;
string input, logString, output;
int length, offset, endIndex;
bool collisionFound = false;

void writelogString(string text) {
	cout << text;
	logStringFile << text;
}

string IntToString(uint64_t number) {
	stringstream stream;
	stream << hex << number;
	string result(stream.str());
	return result;
}

uint64_t StringToInt(string text)
{
	uint64_t temp;
	string::size_type sz = 0;
	temp = stoull(text, &sz, 16);
	return temp;
}

#ifdef MULTITHREAD
 bool findCollision(int order)
 {
	uint64_t startIndex = order == 0 ? 0 : uint64_t(round(order * (filter_io / THREAD_NUM)));
	uint64_t endIndex = ++order == THREAD_NUM ? filter_io : uint64_t(round(order * (filter_io / THREAD_NUM)));
	for (uint64_t i = startIndex; i < endIndex; ++i) {
		if (collisionFound) return false;
		if (result == base[i]) {
			logString += "lenght=" + to_string(length) + "\n""first_pair=" + IntToString(base[i - 1]) + " " + IntToString(base[i]) + "\n";
			input = IntToString(base[filter_io - 1]);
			output = IntToString(result);
			collisionFound = true;
			return true;
		}
	}
	return false;
}
#endif

int main() {
	uint64_t iterations = 0;
	base = new uint64_t[PREDICTED_SIZE];
	output = "abc";
	filter_io = 0;

	bloom_parameters params;
	params.projected_element_count = PREDICTED_SIZE;
	params.false_positive_probability = 0.005;
	params.random_seed = 0xA5A5A5A5;
	params.compute_optimal_parameters();
	bloom_filter filter(params);

	const int threads = thread::hardware_concurrency();
	ThreadPool pool(threads);

	cout << "Enter number of bites: ";
	cin >> length;
#ifdef MULTITHREAD
	logString = "\nthreads=1+" + to_string(threads) + "\n";
#else
	logString = "\nthreads=1\n";
#endif
	logString += "filter_accuracy=" + to_string(0.005) + "\n";
	clock_t begin = clock();

	for (;;)
	{
		input = output;
		output = sha256(output).substr(0, length / 4);
		result = StringToInt(output);
		++filter_io;
		if (filter.contains(output))
		{
			++iterations;
			{
#ifndef MULTITHREAD
				for (uint64_t i = 0; i < filter_io; ++i)
					if (result == base[i]) {
						logString += "lenght=" + to_string(length) + "\n""first_pair=" + IntToString(base[i - 1]) + " " + IntToString(base[i]) + "\n";
						input = IntToString(base[filter_io - 1]); output = IntToString(result); collisionFound = true;
						break;
					}
#else			
				/*thread iterators[THREAD_NUM];
				for (short i = 0; i < THREAD_NUM; ++i)
				iterators[i] = thread(findCollision, i);

				for (short i = 0; i < THREAD_NUM; ++i)
					if (iterators[i].joinable()) iterators[i].join();
				/*/
				for (unsigned short i = 0; i < threads; ++i) {
					function<void()> task = [i, threads]()
					{
						short int order = i;
						uint64_t startIndex = order == 0 ? 0 : uint64_t(round(order * (filter_io / threads)));
						uint64_t endIndex = ++order == threads ? filter_io : uint64_t(round(order * (filter_io / threads)));
						for (uint64_t j = startIndex; j < endIndex; ++j) {
							if (collisionFound) break;
							if (result == base[j]) {
								logString += "lenght=" + to_string(length) + "\n""first_pair=" + IntToString(base[j - 1]) + " " + IntToString(base[j]) + "\n";
								input = IntToString(base[filter_io - 1]);
								output = IntToString(result);
								collisionFound = true;
							}
						}
					};
					pool.AddJob(task);
				}
				pool.WaitAll();//*/
#endif
				if (collisionFound) break;
			}
		}
		if (filter_io == PREDICTED_SIZE) {
			logString += "No collision found!!!\n";
			break;
		}
		base[filter_io] = result;
		filter.insert(output);
		output = IntToString(result);
	} 
	pool.JoinAll();
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	logStringFile.open("performance_logString.txt", ios::out | ios::app);
	writelogString(logString + "second_pair=" + input + " " + output + "\ntime=" + to_string(elapsed_secs) + "\nfilter_io=" + to_string(filter_io) + "\narray_io=" + to_string(iterations) + "\n");
	logStringFile.close();
	delete[] base;
	system("PAUSE");
	return 0;
}

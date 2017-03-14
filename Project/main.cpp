#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <future>

#include <chrono>

// adapted to object oriented library, source: http://www.zedwood.com/article/cpp-sha256-function
#include "sha256.h"
// removed redundant statements, source: https://github.com/ArashPartow/bloom
#include "bloom_filter.hpp"
// source: https://github.com/stfx/ThreadPool2
#include "ThreadPool.h"

using namespace std;

// bit length, number of used threads for search hashInt in array
int bitLength, threads;

// base - array of items, maxItems - array size, hashInt = int representation of hash
// filter_io - number of array_io over filter, array_io - number of array_io over array
uint64_t *base, maxItems, hashInt, filter_io = 0, array_io = 0;

//  hashString - actual hash in string, logString - string with log informations
string hashString, logString;

// flag of collision searching process
volatile bool collisionFound = false;

// time of programme 
double elapsed_secs;

// start time of programme
clock_t startTime;

// SHA256 computer object
SHA256 *hasher;

// pointer of threadpool used for searching in array
ThreadPool *pool;

// array of bloom filter
//vector<bloom_filter> filters;
bloom_filter *filter;

/*
 * Logging function used for write log data to console & log file
 */
void writelogString(string text) {
	ofstream logStringFile;
	logStringFile.open("performance_logString.txt", ios::out | ios::app);
	logStringFile << text;
	logStringFile.close();
	cout << text;
}

/*
 *  Function for converting hash stored in uint64 to string
 */
string IntToString(uint64_t number) {
	static char const digits[] = "0123456789abcdef";
	string results;
	while (number != 0 || results.size() < 1) {
		results += digits[number % 16];
		number /= 16;
	}
	reverse(results.begin(), results.end());
	while (results.length() < bitLength / 4) {
		results.insert(0, "0");
	}
	return results;
}

/*
*  Function for converting hash stored in uint64 to string
*/
uint64_t StringToInt(string text) {
	return stoull(text, nullptr, 16);
}

/*
*  Initalize parameters for bloom filter
*/
void InitBloomFilter(double falseProbability) {
	bloom_parameters params;
	params.projected_element_count = maxItems;
	params.false_positive_probability = falseProbability;
	params.random_seed = 0xA5A5A5A5;
	params.compute_optimal_parameters();
	filter = new bloom_filter(params);
}

/*
*  Useless function for time control, just for fun
*/
void Timer(bool start) {
	if (start)
		startTime = clock();
	else {
		clock_t end = clock();
		elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
	}
}

/*
*  Initalize parameters for thread pool
*  Start programme timer
*/
void InitThreadPool(unsigned threadNum) {
	threads = threadNum <= 0 ? thread::hardware_concurrency() : threadNum;
	pool = new ThreadPool(threads);
	logString = "\nthreads=1+" + to_string(threads) + "\n" + "filter_accuracy=" + to_string(0.005) + "\n";
	Timer(true);
}

/*
*  Initalize system variables
*/
void InitVariables(int lenght, long long predictedSize) {
	hashString = "ahoj";
	maxItems = predictedSize;
	base = new uint64_t[predictedSize];
	bitLength = lenght;
	hasher = new SHA256();
}

/*
 * Function for iteration over array of hashes in uint64_t 
 */
void ArrayIteration(short int i) {
	short int order = i;
	uint64_t startIndex = order == 0 ? 0 : uint64_t(round(order * (filter_io / threads)));
	uint64_t endIndex = ++order == threads ? filter_io : uint64_t(round(order * (filter_io / threads)));
	for (uint64_t j = startIndex; j < endIndex; ++j) {
		if (collisionFound) break;
		if (hashInt == base[j]) {
			logString += "lenght=" + to_string(bitLength) + "\n""first_pair=" + IntToString(base[j - 1]) 
					  + " " + IntToString(base[j]) + "\n" + "second_pair=" + IntToString(base[filter_io - 1])
					  + " " + IntToString(hashInt);
			collisionFound = true;
		}
	}
};

/*
 *  Searching routine in filter
 *  If positive hashInt from filter, call iteration of array,
 *  if filter_io reached filter size, programm ends
 */
void ChainingRoutine() {
	for (;;) {
		hashString = hasher->ComputeHash(hashString).substr(0, bitLength / 4);
		hashInt = StringToInt(hashString);
		++filter_io;
		if (filter_io % 20000000 == 0)
			cout << "filter_io: " << to_string(filter_io/1000000) << endl;
		if (filter->contains(hashInt))
		{
			++array_io;
			if(array_io % 5000 == 0)
				cout << "array_io: " << to_string(array_io/1000) << endl;
			{
				for (short int i = 0; i < threads; ++i) {
					function<void()> task = [i]() {ArrayIteration(i); };
					pool->AddJob(task);
				}
				pool->WaitAll();
				if (collisionFound) return;
			}
		}
		if (filter_io == maxItems) {
			logString = "\nArray size limit reached! No collision found! Program ends!\n";
			return;
		}
		base[filter_io] = hashInt;
		filter->insert(hashInt);
	}
}

/*
* Function for clean memory and write log string
*/
void Cleanup() {
	pool->JoinAll();
	Timer(false);
	if (logString.length() != 61)
		logString += "\ntime=" + to_string(elapsed_secs) + "\nfilter_io=" + to_string(filter_io)
		+ "\narray_io=" + to_string(array_io) + "\n";
	writelogString(logString);
	delete[] base;
	system("PAUSE");
}

/*
 * Main function of programme
 * order of parameters:
 * 0-Program 1-bites 2-falseProbability 3-predictedSize 4-threads(optional)
 */
int main(int argc, char* argv[]) {
	InitVariables(atoi(argv[1]), atoll(argv[3]));
	InitBloomFilter(atof(argv[2]));
	InitThreadPool(argc > 4 ? atoi(argv[4]) : 0);
	ChainingRoutine();
	Cleanup();
	return 0;
}

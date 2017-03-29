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
int bitLength = 32, threads = 0;

// base - array of items, maxItems - array size, hashInt = int representation of hash
// filter_io - number of array_io over filter, array_io - number of array_io over array
uint64_t *base, maxItems, hashInt, filter_io = 0, array_io = 0;

//  hashString - actual hash in string, logString - string with log informations
string hashString = "ahoj", logString = "";

// flag of collision searching process
volatile bool collisionFound = false;

// time of programme, false probability of bloom filter
double elapsed_secs, falseProbability = 0.005;

// start time of programme
clock_t startTime;

// SHA256 computer object
SHA256 *hasher;

// pointer of threadpool used for searching in array
ThreadPool *pool;

// array of bloom filter
bloom_filter *filter;

// enable logging
bool logging = false;

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
uint64_t (*StringToInt)(string) = [](string text){
	return stoull(text, nullptr, 16);
};

/*
*  Initalize parameters for bloom filter
*/
void InitBloomFilter() {
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
void InitThreadPool() {
	pool = new ThreadPool(threads);
	logString = "\nthreads=1+" + to_string(threads) + "\n" + "filter_accuracy=" + to_string(falseProbability) + "\n";
	Timer(true);
}

/*
 * Function for iteration over array of hashes in uint64_t 
 */
function<void(short)> ArrayIteration = [](short i) {
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
		if (logging && filter_io % 10000000 == 0)
			cout << "filter_io: " << to_string(filter_io/1000000) << endl;
		if (filter->contains(hashInt))
		{
			++array_io;
			if(logging && array_io % 1000 == 0)
				cout << "array_io: " << to_string(array_io/1000) << endl;
			{
				for (short int i = 0; i < threads; ++i) {
					pool->AddJob(bind(ArrayIteration,i));
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
	delete pool; 
	delete hasher;
	delete filter;
	delete[] base;
	system("PAUSE");
}

/*
*  Initalize system variables from cmd arguments
*/
void InitVariables(int argc, char* argv[])
{
	threads = thread::hardware_concurrency();
	for(int i = 1; i < argc; ++i)
	{
		switch(argv[i][1])
		{
		case 'i': {hashString = argv[++i]; break;}
		case 'b': {bitLength = atoi(argv[++i]); break;}
		case 'p': {falseProbability = atof(argv[++i]); break;}
		case 'c': {maxItems = atoll(argv[++i]) * 1000000; break;}
		case 't': {threads = atoi(argv[++i]); break;}
		case 'l': {logging = true; break;}
		default: ;
		}
	}
	base = new uint64_t[maxItems];
	hasher = new SHA256();
}

/*
 * Main function of programme
 * order of parameters:
 * 0-Program 1-input 2-bites 3-falseProbability
 * 4-predictedSize(in M) 5-threads(optional) 6-logging(optional)
 */
int main(int argc, char* argv[]) {
	InitVariables(argc, argv);
	InitBloomFilter();
	InitThreadPool();
	ChainingRoutine();
	Cleanup();
	return 0;
}

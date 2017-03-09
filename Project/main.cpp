#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

#include "sha256.h"
#include "bloom_filter.hpp"
#include "ThreadPool.h"

using namespace std;

// bit length, number of used threads for search result in array
int bitLength, threads;

// base - array of items, maxItems - array size, result = int representation of hash
// filter_io - number of iterations over filter, array_io - number of iterations over array
uint64_t *base, maxItems, result, filter_io, iterations;

// input - last output copy for log, output - actual hash in string, logString - string with log informations
string input, output, logString;

// flag of collision searching process
bool collisionFound = false;

// time of programme 
double elapsed_secs;

// start time of programme
clock_t startTime;

// pointer of threadpool used for searching in array
ThreadPool *pool;

// pointer of bloom filter
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
	stringstream stream;
	stream << hex << number;
	string result(stream.str());
	return output;
}

/*
*  Function for converting hash stored in uint64 to string
*/
uint64_t StringToInt(string text) {
	uint64_t temp;
	string::size_type sz = 0;
	temp = stoull(text, &sz, 16);
	return temp;
}

/*
*  Initalize parameters for bloom filter
*/
void InitBloomFilter(double falseProbability, unsigned long long predictedSize) {
	bloom_parameters params;
	params.projected_element_count = predictedSize;
	params.false_positive_probability = falseProbability;
	params.random_seed = 0xA5A5A5A5;
	params.compute_optimal_parameters();
	filter = new bloom_filter(params);
}

/*
*  Initalize parameters for thread pool
*/
void InitThreadPool(unsigned threadNum = 0) {
	threads = threadNum == 0 && threadNum < thread::hardware_concurrency() ? thread::hardware_concurrency() : threadNum;
	pool = new ThreadPool(threads);
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
*  Initalize system variables
*/
void InitVariables(int lenght, long long predictedSize) {
	output = "abc";
	filter_io = 0;
	iterations = 0;
	maxItems = predictedSize;
	base = new uint64_t[predictedSize];
	bitLength = lenght;
	logString = "\nthreads=1+" + to_string(threads) + "\n" + "filter_accuracy=" + to_string(0.005) + "\n";
	Timer(true);
}

/*
 * Function for clean memory and write log string
 */
void Cleanup() {
	pool->JoinAll();
	Timer(false);
	if (logString.length() != 61) 
		logString += "second_pair=" + input + " " + output + "\ntime=" + to_string(elapsed_secs)
				  + "\nfilter_io=" + to_string(filter_io) + "\narray_io=" + to_string(iterations) + "\n";
	writelogString(logString);
	delete[] base;
	system("PAUSE");
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
		if (result == base[j]) {
			logString += "lenght=" + to_string(bitLength) + "\n""first_pair=" + IntToString(base[j - 1]) 
					  + " " + IntToString(base[j]) + "\n";
			input = IntToString(base[filter_io - 1]);
			output = IntToString(result);
			collisionFound = true;
		}
	}
};

/*
 *  Searching routine in filter
 *  If positive result from filter, call iteration of array,
 *  if filter_io reached filter size, programm ends
 */
void ChainingRoutine() {
	for (;;) {
		input = output;
		output = sha256(output).substr(0, bitLength / 4);
		result = StringToInt(output);
		++filter_io;
		if (filter->contains(result))
		{
			++iterations;
			{
				for (unsigned short i = 0; i < threads; ++i) {
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
		base[filter_io] = result;
		filter->insert(result);
	}
}

/*
 * Main function of programme
 * order of parameters:
 * 0-Program 1-bites 2-falseProbability 3-predictedSize 4-threads(optional)
 */
int main(int argc, char* argv[]) {
	InitBloomFilter(2 * atof(argv[2]), atoll(argv[3]));
	InitThreadPool(argc > 4 ? atoi(argv[4]) : 0);
	InitVariables(atoi(argv[1]), atoll(argv[3]));
	ChainingRoutine();
	Cleanup();
	return 0;
}

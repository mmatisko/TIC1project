#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#ifdef __linux
#include <algorithm>
#endif
#include "main.hpp"

using namespace std;

/*
 * Logging function used for write log data to console & log file
 */
void writelogString(string text) {
	ofstream logStringFile;
	try {
		logStringFile.open("performance_logString.txt", ios::out | ios::app);
		logStringFile << text;
		logStringFile.close();
	} catch(exception e)
	{
		throw new exception("Log file exception, cannot open or close file.");
	}
	cout << text;
}

/*
 * Initalize parameters for bloom filter
 */
void InitBloomFilter(uint64_t count) {
	bloom_parameters params;
	params.projected_element_count = count;
	params.false_positive_probability = falseProbability;
	params.random_seed = 0xA5A5A5A5;
	params.compute_optimal_parameters();
	filter = new bloom_filter(params);
}

/*
 * Initalize parameters for thread pool
 * Start programme timer
 */
void InitThreadPool() {
	pool = new ThreadPool(threads);
	logString = "\ninput=" + hashString + "\nthreads=1+" + to_string(threads) + "\n" + "filter_accuracy=" + to_string(falseProbability) + "\n";
	timer = new Timer();
}

/*
 * Function for iteration over array of hashes in uint64_t 
 */
function<void(short)> ArrayIteration = [](short order) {
	uint64_t startIndex = order == 0 ? 0 : uint64_t(round(order * (floor(filter_io / mod) / threads)));
	uint64_t endIndex = ++order == threads ? uint64_t(ceil(filter_io / mod)) : uint64_t(round(order * (ceil(filter_io / mod) / threads)));
	for (uint64_t j = startIndex; j < endIndex; ++j) {
		if (collisionFound) break;
		if (hashString == base[j]) {

			collisionFound = true; //for quit other threads
			hashString = base[j - 1];
			delete filter;
			delete[] base;
			base = new string[mod + 1];
			InitBloomFilter(mod * multiplier);
			filter_io -= mod;
			int k = 0;
			
			if (ringMode) {
				base[0] = hashString;
				++k;
			}
			
			for (k; k < mod + 1;) { //compute hashes from tails
				hashString = hasher->ComputeHash(hashString).substr(0, bitLength / 4);
				filter->insert(hashString);
				base[k++] = hashString;
			}

			hashString = backupString;
			for (k = 0; k < 2 * mod; ++k) { //compute and compare hashes from ring with hashes from tail
				hashString = hasher->ComputeHash(hashString).substr(0, bitLength / 4);
				if (filter->contains(hashString)) {
					for (int i = 0; i < 2 * mod; ++i)
					{
						++filter_io;
						if (hashString == base[i]) {
							logString += "lenght=" + to_string(bitLength) + "\n""first_pair=" + base[i - 1]
								+ " " + base[i] + "\n" + "second_pair=" + backupString
								+ " " + hashString;
							return;
						}
					}
				}
				backupString = hashString;
			}
		}
	}
};

/*
 * Searching routine in filter
 * If positive hashInt from filter, call iteration of array,
 * if filter_io reached filter size, programm ends
 */
void ChainingRoutine() {
	string back;

	if(ringMode)
	{
		++filter_io;
		base[0] = hashString;
	}

	for (;;) {
		hashString = hasher->ComputeHash(hashString).substr(0, bitLength / 4);

		if (logging && filter_io % 10000000 == 0 && filter_io > 0) {
			cout << "filter_io: " << to_string(filter_io / 1000000) << endl;
		}

		if (filter->contains(hashString))
		{
			++array_io;
			if (logging && array_io % 1000 == 0)
				cout << "array_io: " << to_string(array_io / 1000) << endl;
			{
				for (short int i = 0; i < threads; ++i) {
					pool->AddJob(bind(ArrayIteration, i));
				}
				pool->WaitAll();
				if (collisionFound)
				{
					timer->Stop();
					if (logString.length() != 61)
						logString += timer->GetStringTime() + "\nfilter_io=" + to_string(filter_io)
						+ "\narray_io=" + to_string(array_io) + "\n";
					return;
				}
			}
		}
		if (filter_io == capacity) {
			logString = "\nArray size limit reached! No collision found! Program ends!\n";
			return;
		}
		
		if (filter_io % mod == 0) {
			base[filter_io / mod] = hashString;
			filter->insert(hashString);
			backupString = back;
			back = hashString;
		}
		++filter_io;
	}
}

/*
 * Function for clean memory and write log string
 */
void Cleanup() {
	pool->JoinAll();
	writelogString(logString);
	delete pool; 
	delete hasher;
	delete filter;
	delete[] base;
	getchar();
}

/*
 * Initalize system variables from cmd arguments
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
		case 'c': {capacity = atoll(argv[++i]) * 1000000; break;}
		case 't': {threads = atoi(argv[++i]); break;}
		case 'l': {logging = true; break;}
		case 'r': {ringMode = true; break; }
		case 'm': {multiplier = atoi(argv[++i]); break;}
		case 'n': {mod = atoi(argv[++i]); break;}
		default: ;
		}
	}
	base = new string[capacity / mod];
	hasher = new SHA256();
}

/*
 * Main function of programme
 */
int main(int argc, char* argv[]) {
	InitVariables(argc, argv);
	InitBloomFilter(capacity / mod * multiplier);
	InitThreadPool();
	ChainingRoutine();
	Cleanup();
	return 0;
}

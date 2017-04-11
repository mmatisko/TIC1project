#pragma once

// adapted to object oriented library, source: http://www.zedwood.com/article/cpp-sha256-function
#include "sha256.h"

// removed redundant statements, source: https://github.com/ArashPartow/bloom
#include "bloom_filter.hpp"

// source: https://github.com/stfx/ThreadPool2
#include "ThreadPool.h"

// small custom library for measuring time of running programme
#include "Timer.hpp"


// bit length of collision hash
int bitLength = 32;

// number of used threads for search hashInt in array
int threads = 0;

// multiplier of filter size over part of array size
int multiplier = 5;

// modulo of saved hash
int mod = 10;

// array of itemshashInt => int representation of has
uint64_t *base;

// size of array 
uint64_t capacity;

// capacity - array size
uint64_t hashInt;

// filter_io - number of array_io over filter
uint64_t filter_io = 0;

// array_io - number of array_io over array
uint64_t array_io = 0;

// size of one filter in filter array
uint64_t filterSize = 0;

// actual hash in string 
string hashString = "ahoj";

// last used hash in string
string backupString = "";

// string with log informations
string logString = "";

// flag of collision searching process
bool collisionFound = false;

// false probability of bloom filter
double falseProbability = 0.005;

// stopwatch for programme
Timer *timer;

// SHA256 computer object
SHA256 *hasher;

// pointer of threadpool used for searching in array
ThreadPool *pool;

// parameters for bloom filter
bloom_parameters *params;

// array of bloom filter
bloom_filter *filter;

// enable logging
bool logging = false;


// Main function of programme
int main(int argc, char* argv[]);

// Initalize system variables from cmd arguments
void InitVariables(int argc, char* argv[]);

//Function for clean memory and write log string
void Cleanup();

/*
*  Searching routine in filter
*  If positive hashInt from filter, call iteration of array,
*  if filter_io reached filter size, programm ends
*/
void ChainingRoutine();

//Function for iteration all filters in filter array
//void FindInFilter();

/*
*  Initalize parameters for thread pool
*  Start programme timer
*/
void InitThreadPool();

// Initalize parameters for bloom filter
void InitBloomFilter();

// Function for converting hash stored in uint64 to string
uint64_t StringToInt(string text);

// Function for converting hash stored in uint64 to string
string IntToString(uint64_t number);

// Logging function used for write log data to console & log file
void writelogString(string text);

/*
* Function for iteration over one filter of uint64_t items in filter array
*/
extern function<void(short)> FilterIteration;

//Function for iteration over array of hashes in uint64_t
extern function<void(short)> ArrayIteration;
#pragma once

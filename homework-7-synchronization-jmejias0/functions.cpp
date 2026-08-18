/*
 * Name: Jose Mejias
 * CWID: 820507572
 * Email: jmejias0@csu.fullerton.edu
 */

/**
 * This is where you'll place your function definitions.
 * 
 * Note that functions.hpp contains prototypes for each function listed below.
 * 
 * You may choose to create additional helper functions if you wish,
 * 	but their prototypes must also be added to functions.hpp
 * 
 */

/**
 * Header includes
 * The first header include has been added for you.
 * You may add additional headers below, if needed
 */
#include "functions.hpp"
#include <cstddef>
#include <pthread.h>
#include <fstream>
#include <stdexcept>
//	TODO: More header includes?


/**
 * Globals
 * We're using globals in this Assignment for simplicity
 * (they will allow us to avoid using classes)
 */

/********************************************
 * Probably don't want to mess with this area
 */
bool G_USE_MUTEXES = true;
const char * G_FILE_NAME = "sync.txt";
const size_t G_LINE_COUNT_PER_TYPE = 250000;
/********************************************/

pthread_mutex_t my_mutex_isCool = PTHREAD_MUTEX_INITIALIZER;
/**
 * If you need to add custom globals, add them here:
 */
// TODO: More globals?


/**
 * beginWriting()
 * 
 * Consider this function the main entry point for your code.
 * 
 * First, it should look at the incoming value of use_mutexes,
 *	and assign it to the global variable above, G_USE_MUTEXES.
 * 
 * Next, it should spawn two threads: One for each of Thread-A and Thread-B.
 * Each thread enters its same-named function.
 * If a thread fails to launch, throw an std::runtime_error!
 * 
 * The function should then join each thread before returning.
 */
void beginWriting(bool use_mutexes)
{
	//	TODO: Your code here
	G_USE_MUTEXES = use_mutexes;
	std::ofstream file(G_FILE_NAME, std::ios::trunc); // Empty the file, if does't exits, create it, if exists wipe old content
	file.close();					  // close file
	
	pthread_t threadA;
	pthread_t threadB;

	int resultA = pthread_create(&threadA, nullptr, threadAEntry, nullptr);
	if (resultA != 0) {
		throw std::runtime_error("Failed to create thread-A");
	}

	int resultB = pthread_create(&threadB, nullptr, threadBEntry, nullptr);
	if (resultB != 0) {
		throw std::runtime_error("Failed to create thread-B");
	}

	pthread_join(threadA, nullptr);
	pthread_join(threadB, nullptr);

}


/**
 * threadAEntry()
 * 
 * This is the entry point for Thread-A
 * 
 * It should write one type of line to your output file, many times.
 * (in fact, G_LINE_COUNT_PER_TYPE times)
 * 
 * It should have the ability to choose whether to use mutexes, or not
 * Have it examine the value of the global variable G_USE_MUTEXES,
 * 	and use knowledge to decide whether to lock/unlock mutexes, or not.
 * 
 * You can read the global variable G_FILE_NAME for the target file name.
 * 
 * If you get a warning about the return value, just return nullptr or NULL
 */
void* threadAEntry(void* param)
{
	//	TODO: Your code here
	std::ofstream file(G_FILE_NAME, std::ios::app);	// open file in append mode


	// write 250000 number lines, one at a time with space after each character
	for (size_t line = 0; line <G_LINE_COUNT_PER_TYPE; ++line) {
		if (G_USE_MUTEXES) {
			pthread_mutex_lock(&my_mutex_isCool);
		}

		for (char ch = '0'; ch <= '9'; ++ch) {
			file << ch;
			file << ' ';
			file.flush();
		}

		file << '\n';
		file.flush();

		if (G_USE_MUTEXES) {
			pthread_mutex_unlock(&my_mutex_isCool);
		}
	}

	return nullptr;
}

/**
 * threadBEntry()
 * 
 * This function should behave the same way as threadAEntry(),
 * 	but write a different type of line.
 */
void* threadBEntry(void* param)
{
	//	TODO: Your code here
	std::ofstream file(G_FILE_NAME, std::ios::app);

	for (size_t line = 0; line < G_LINE_COUNT_PER_TYPE; ++line) {
		if (G_USE_MUTEXES) {
			pthread_mutex_lock(&my_mutex_isCool);
		}

		for (char ch = 'A'; ch <= 'Z'; ++ch) {
			file << ch;
			file << ' ';
			file.flush();
		}

		file << '\n';
		file.flush();

		if (G_USE_MUTEXES) {
			pthread_mutex_unlock(&my_mutex_isCool);
		}
	}

	return nullptr;
}














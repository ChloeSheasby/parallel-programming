// MonteCarlo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <omp.h>
#include <iostream>

/*
The purpose of this code is to ensure that OpenMP is working and running on multiple threads.
*/
using namespace std;

int main(int argc, char* argv[])
{
	// This statement should only print once
	printf("Starting Program!\n");

#pragma omp parallel
	{
		// This statement will run on each thread.
		// If there are 4 threads, this will execute 4 times in total
		printf("Running on multiple threads\n");
	}

	// We're out of the parallelized section.
	// Therefore, this should execute only once
	printf("Finished!\n");

	cin.get();
	return 0;
}
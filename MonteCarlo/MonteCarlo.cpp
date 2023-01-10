#include <omp.h>
#include <iostream>
using namespace std;

double getRand();
long double Calculate_Pi_Sequential(long long);
long double Calculate_Pi_Parallel(long long);

int main() {
	clock_t start, end;

	long long num_tosses = 10000000;

	printf("Timing sequential...\n");		
	start = clock();
	long double sequential_pi = Calculate_Pi_Sequential(num_tosses);
	end = clock();
	printf("Took %f seconds\n\n", (double)(end - start) / CLOCKS_PER_SEC);

	printf("Timing parallel...\n");
	start = clock();
	long double parallel_pi = Calculate_Pi_Parallel(num_tosses);
	end = clock();
	printf("Took %f seconds\n\n", (double)(end - start) / CLOCKS_PER_SEC);

	// This will print the result to 10 decimal places
	printf("Pi = %.10Lf (sequential)\n", sequential_pi);
	printf("Pi = %.10Lf (parallel)\n", parallel_pi);

	return 0;
}

// Returns a random value between -1 and 1
double getRand() {
	return (double)rand() * 2 / (double)(RAND_MAX)-1;
}

long double Calculate_Pi_Sequential(long long number_of_tosses) {
	long double rand_x, rand_y, origin_dist, pi;
	int circle_points = 0;

	srand(time(NULL));

	for (int i = 0; i < number_of_tosses; i++) {

		rand_x = getRand();
		
		rand_y = getRand();

		origin_dist = rand_x * rand_x + rand_y * rand_y;

		if (origin_dist <= 1)
			circle_points++;
	}

	pi = double(4 * circle_points) / number_of_tosses;

	return pi;
}

long double Calculate_Pi_Parallel(long long number_of_tosses) {
	long double rand_x, rand_y, origin_dist, pi;
	int circle_points = 0;

	srand(time(NULL) + omp_get_thread_num());

# pragma omp parallel for num_threads(omp_get_max_threads()) reduction(+:circle_points) private(rand_x, rand_y, origin_dist)
	for (int i = 0; i < number_of_tosses; i++) {

		rand_x = getRand();

		rand_y = getRand();

		origin_dist = rand_x * rand_x + rand_y * rand_y;

		if (origin_dist <= 1)
			circle_points++;
	}

	pi = double(4 * circle_points) / number_of_tosses;
	return pi;
}
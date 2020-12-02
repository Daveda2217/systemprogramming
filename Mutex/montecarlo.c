#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; // mutex
int point_in_circle = 0;

void *thread_point(void *arg)
{									 // thread function
	int point_total = *((int *)arg); // parameter
	srand(time(NULL));				 // get random seed
	for (int i = 1; i <= point_total; i++)
	{
		pthread_mutex_lock(&m); // lock
		double x = rand() / (double)RAND_MAX;
		double y = rand() / (double)RAND_MAX;

		if ((x * x + y * y) <= 1)
		{
			point_in_circle += 1;
		}

		pthread_mutex_unlock(&m); // unlock
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int num_t = atoi(argv[1]);	 // # of thread
	int num_rpt = atoi(argv[2]); // # of random point per thread

	pthread_t tid[num_t];

	for (int i = 0; i < num_t; i++)
	{
		pthread_create(&tid[i], NULL, thread_point, (void *)&num_rpt);
	}

	for (int i = 0; i < num_t; i++)
	{
		pthread_join(tid[i], NULL);
	}

	double pi = 4 * ((double)point_in_circle / ((double)num_t * (double)num_rpt));
	printf("%lf\n", pi);
	return 0;
}

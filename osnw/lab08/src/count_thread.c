#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_THREAD 2

/* 스레드 예제5) 스레드 카운터  

  메인 스레드의 count 변수를 , 워커 스레드들이 참조하여 +1 씩 올려주고 있다. 
  이 예제에서는 race condition문제가 발생하지 않는다.
*/
void *t_func(void *data)
{
	int *count = (int *)data; // typecasting
	int tmp;
	pthread_t thread_id = pthread_self(); // get my thread id

	while (1) // infinitly count up
	{
		printf("%lu %d\n", thread_id, *count);
		*count = *count + 1;
		sleep(1);
	}
}

int main(int argc, char **argv)
{
	pthread_t thread_id[MAX_THREAD];
	int i = 0;
	int count = 0;

	for (i = 0; i < MAX_THREAD; i++)
	{
		// 같은 함수 스레드를 2개 만듦
		pthread_create(&thread_id[i], NULL, t_func, (void *)&count);
		usleep(5000); // sleep(ms단위)
	}

	while (1)
	{
		printf("Main Thread : %d\n", count);
		sleep(2);
	}
	for (i = 0; i < MAX_THREAD; i++)
	{
		pthread_join(thread_id[i], NULL);
	}
	return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_THREAD 2

/* 스레드 예제6) 스레드 카운터  (+레이스 컨디션)

  메인 스레드의 count 변수를 , 워커 스레드들이 참조하여 +1 씩 올려주고 있다. 
  이 예제에서는 race condition문제가 발생한다.
  2개의 워커 스레드가 count변수에 동시에 접근하게 된다.
  이를 다음 예제에서는 한 스레드만 CS에 접근하도록 mutex를 걸자
	...
	281473203630560 7
	281473195237856 7
	Main Thread : 8
	...
*/
void *t_func(void *data)
{
	int *count = (int *)data;
	int tmp;
	pthread_t thread_id = pthread_self();

	while (1)
	{
		tmp = *count;
		tmp++;
		sleep(1);
		*count = tmp;
		printf("%lu %d\n", thread_id, *count);
	}
}

int main(int argc, char **argv)
{
	pthread_t thread_id[MAX_THREAD];
	int i = 0;
	int count = 0;

	for (i = 0; i < MAX_THREAD; i++)
	{
		pthread_create(&thread_id[i], NULL, t_func, (void *)&count);
		usleep(5000);
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

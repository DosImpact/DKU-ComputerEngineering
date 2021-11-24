#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ARRAY_SIZE 100
#define THREAD_NUM 4

/* 스레드 예제 8) 스레드간 동기화 ( signal & wait )

*/

pthread_mutex_t t_lock; // mutux lock 설정
pthread_cond_t t_cond;	// 컨디션 설정

int *data_array;
int sum_array[THREAD_NUM];

struct data_info
{
	int *d_point;
	int idx;
};

void *t_func(void *data)
{
	struct data_info d_info;
	int i = 0;
	int sum = 0;
	d_info = *((struct data_info *)data);

	// TODO : 왜 이 변수가 여기 있으면 스레드 3이 2번 나올까?(랜덤하게)
	// printf("Wait %d Thread\n", d_info.idx);
	pthread_mutex_lock(&t_lock);		 // --- CS 섹션 START
	pthread_cond_wait(&t_cond, &t_lock); // > wait 조건 변수를 사용하는 영역을 mutex로 보호
	printf("Start %d Thread\n", d_info.idx);
	pthread_mutex_unlock(&t_lock); // --- CS 섹션 END

	for (i = 0; i < 25; i++)
	{
		sum += d_info.d_point[(d_info.idx * 25) + i];
	}

	printf("(%d) %d\n", d_info.idx, sum);
	sum_array[d_info.idx] = sum;
	return NULL;
}

int main(int argc, char **argv)
{
	int i = 0;
	int sum = 0;
	struct data_info d_info;

	pthread_t thread_id[THREAD_NUM];

	if ((data_array = malloc(sizeof(int) * ARRAY_SIZE)) == NULL)
	{
		perror("Malloc Failuer");
		return 1;
	}

	pthread_mutex_init(&t_lock, NULL);
	pthread_cond_init(&t_cond, NULL);

	for (i = 0; i < THREAD_NUM; i++)
	{
		d_info.d_point = data_array;
		d_info.idx = i;

		pthread_create(&thread_id[i], NULL, t_func, (void *)&d_info);
		usleep(100);
	}

	for (i = 0; i < ARRAY_SIZE; i++)
	{
		*data_array = i;
		*data_array++;
	}

	pthread_cond_broadcast(&t_cond); // 모든 스레드에게 시그널 보내기(그만 wait해라)
	for (i = 0; i < THREAD_NUM; i++)
	{
		pthread_join(thread_id[i], NULL);
		sum += sum_array[i];
	}
	printf("SUM (0-99) : %d\n", sum);
	return 0;
}

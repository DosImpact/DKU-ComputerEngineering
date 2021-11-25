#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

union semun
{
	int val;
};

int main(int argc, char **argv)
{
	int shmid; // 공유메모리 아이디
	int semid; // 세마포어 아이디

	int *cal_num;
	void *shared_memory = NULL;
	union semun sem_union; // 세마포어 구조체

	struct sembuf semopen = {0, -1, SEM_UNDO}; // P 연산
	struct sembuf semclose = {0, 1, SEM_UNDO}; // V 연산

	// 공유메모리 생성
	shmid = shmget((key_t)1234, sizeof(int), 0666 | IPC_CREAT);
	if (shmid == -1)
	{
		return 1;
	}

	// 세마포어 생성
	semid = semget((key_t)3477, 1, IPC_CREAT | 0666);
	if (semid == -1)
	{
		return 1;
	}

	// 공유id 로부터 공유메모리 주소값 받기
	shared_memory = shmat(shmid, NULL, 0);
	if (shared_memory == (void *)-1)
	{
		return 1;
	}

	// typecasting
	cal_num = (int *)shared_memory;

	// 세마포어 설정(1개 만들기)
	sem_union.val = 1;
	if (-1 == semctl(semid, 0, SETVAL, sem_union))
	{
		return 1;
	}

	while (1)
	{
		int local_var = 0;
		if (semop(semid, &semopen, 1) == -1) // P operator
		{
			return 1;
		}
		//************ critical section ************
		local_var = *cal_num + 1;
		sleep(1);
		*cal_num = local_var;
		printf("Producer semaphore : %d\n", *cal_num);
		//************ critical section end *********
		semop(semid, &semclose, 1); // V operator
	}
	return 1;
}

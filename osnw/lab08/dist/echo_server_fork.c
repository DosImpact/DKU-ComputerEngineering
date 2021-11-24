#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAXLINE 1024
#define PORTNUM 3600

union semun
{
	int val;
};
struct cal_data
{
	char char_data[50];
	int num_data;
};

int main(int argc, char **argv)
{
	int listen_fd, client_fd = 0;
	pid_t pid;
	socklen_t addrlen;
	int readn;

	int rfd, wfd;								 // IPC - 읽기,쓰기 pipe
	char buf_name_pipe[MAXLINE];				 // IPC - 데이터 버퍼 (unused)
	char buf_name_pipe_list[3][MAXLINE];		 //  IPC - 데이터 버퍼 , 3개의 클라이언트에서 온 데이터를 저장.
	char buf_name_pipe_list_strcat[MAXLINE * 3]; // 3개의 자식 Process 에서 온 데이터를 합친 결과를 저장

	char buf_list[3][MAXLINE];	  // 3개의 클라이언트에서 온 데이터를 저장.
	char buf_strcat[MAXLINE * 3]; // 3개의 클라이언트에서 온 데이터를 합친 결과를 저장

	struct sockaddr_in client_addr, server_addr;

	/* step1. Server setting & bind * listen */

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return 1;
	}
	memset((void *)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);

	if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind error");
		return 1;
	}
	if (listen(listen_fd, 5) == -1)
	{
		perror("listen error");
		return 1;
	}

	signal(SIGCHLD, SIG_IGN);
	while (1)
	{
		addrlen = sizeof(client_addr);
		/* step2. client accept & fork */
		// 3개의 클라이언트와 먼저 접속 후 , fork를 진행합니다.
		for (int i = 1; i < 100; i++)
		{
			client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);
			if (client_fd == -1)
			{
				printf("accept error\n");
				break;
			}
			printf("client[%d] is connected & fork \n", i);
			pid = fork();
			if (pid == 0) //case:child
			{
				break;
			}
			else if (pid > 0) //case:parent
			{
				continue;
			}
		}

		/* step3. IPC */
		if (pid == 0) //case:child
		{
			// child에선 더 이상 accept할 일이 없으므로 서버 소켓 종료
			close(listen_fd);

			// [1] 클라이언트로부터 문자열 받기 ( for IPC )
			memset(buf_list[0], 0x00, MAXLINE);
			readn = read(client_fd, buf_list[0], MAXLINE);
			if (readn > 0)
			{
				printf("Read Data (for IPC) %s(%d) : %s", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, buf_list[0]);
				strcat(buf_strcat, buf_list[0]);

				printf("✔ 합처진 문자열 클라이언트로 보내기 %s \n", buf_list[0]);
				write(client_fd, buf_list[0], strlen(buf_list[0]));
			}

			/* 🚀 공유 메모리 및 세마포어 생성 */
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

			/* 🚀 한번 더 consumer producer fork */

			pid = fork();
			if (pid == 0) //case:child
			{
				/* consumer - 공유메모리 값을 읽어서 클라이언트로 보낸다. */
				// 공유메모리 얻기
				shmid = shmget((key_t)1234, sizeof(int), 0666);
				if (shmid == -1)
				{
					perror("shmget failed : ");
					exit(0);
				}
				// 세마포어 얻기
				semid = semget((key_t)3477, 0, 0666);
				if (semid == -1)
				{
					perror("semget failed : ");
					return 1;
				}
				// 공유id 로부터 공유메모리 주소값 받기
				shared_memory = shmat(shmid, NULL, 0);
				if (shared_memory == (void *)-1)
				{
					perror("shmat failed : ");
					exit(0);
				}
				// typecasting
				cal_num = (int *)shared_memory;

				while (1)
				{
					int local_var = 0;
					if (semop(semid, &semopen, 1) == -1) // P operator
					{
						perror("semop error : ");
					}
					//************ critical section ************
					local_var = *cal_num + 1;
					sleep(2);
					*cal_num = local_var;
					printf("Consumer semaphore : %d\n", *cal_num);
					//************ critical section end *********
					semop(semid, &semclose, 1); // V operator

					// 합처진 문자열 클라이언트로 보내기
					printf("✔ 합처진 문자열 클라이언트로 보내기 %s \n", buf_list[0]);
					write(client_fd, buf_list[0], strlen(buf_list[0]));
					// sleep(1);
					// memset(buf_list[0], 0x00, MAXLINE);
				}
				/* 클라이언트 소켓 종료 */
				close(client_fd);
			}
			else if (pid > 0) //case:parent
			{
				/* producer - 받은 데이터를 가공하여 공유메모리에 계속 올린다. */
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
				continue;
			}

			/* (child) IPC PIPE 열기 (주의 w,r 네임드 파이프가 P,C 간에 크로스 되어야함 ) */
			// rfd = open("/tmp/myfifo_w", O_RDWR);
			// if (rfd < 0)
			// {
			// 	perror("read open error\n");
			// 	return 1;
			// }
			// wfd = open("/tmp/myfifo_r", O_RDWR); // open named pipe myfifo_r as write buffer
			// if (wfd < 0)
			// {
			// 	perror("write open error\n");
			// 	return 1;
			// }
			// while (1)
			// {
			// 	// [2] 부모 프로세스에 보내기 ( IPC PIPE)
			// 	printf("> [2] 부모 프로세스에 보내기 \n");
			// 	write(wfd, buf_list[0], strlen(buf_list[0])); // write -> to
			// 	// [3] 부모 프로세스로 부터 합처진 문자열 받기
			// 	read(rfd, buf_name_pipe_list_strcat, MAXLINE); // read
			// 	printf("✔ Data from Parent process -> %s \n", buf_name_pipe_list_strcat);
			// 	break;
			// }
			/* IPC 종료 */
			// close(wfd);
			// close(rfd);
		}
		else if (pid > 0) //case:parent
		{
			break;
		}
	}
	return 0;
}

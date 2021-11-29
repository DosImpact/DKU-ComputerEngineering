#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define THREAD_MAX_NUM 100
#define MAXLINE 1024
#define PORTNUM 3600
/* 스레드 예제 9) 스레드 에코 서버
	- 서버 접속시 accpet은 메인 스레드가 ,
	- 이후 read/write는 서브 스레드가 담당	
*/

pthread_mutex_t t_lock[THREAD_MAX_NUM]; // mutux lock 설정
pthread_cond_t t_cond[THREAD_MAX_NUM];	// 컨디션 설정
int lock_cnt = 0;

struct network_data
{
	int num;
	char content[100];
};
typedef struct thread_data
{
	int sockfd;
	struct network_data ndata;
	int producer_tid;
	int consumer_tid;
	int lock_id;
} PARAMS;

void rotate(char *target)
{
	int arrLen = strlen(target) - 1; //배열의 길이를 저장할 변수
	int rNum = 1;					 //rotate를 몇번할지를 받는 변수
	char *arr;

	// scanf("%d %d", &arrLen, &rNum);
	arr = (char *)malloc(sizeof(char) * strlen(target)); //동적할당.
	strcpy(arr, target);

	for (int i = 0; i < arrLen; i++)
	{
		arr[(i + 1) % arrLen] = target[i];
	}

	//동적할당 해제
	strcpy(target, arr);
	// printf("rotated %s \n", target);
	free(arr);
}

void *thread_func_producer(void *arg)
{
	printf("✔️ producer-thread construced %p \n", arg);
	// PARAMS props = *((PARAMS *)arg);
	// printf("props : %d %d %d \n", props.sockfd, props.lock_id, props.producer_tid);
	// args typecasting
	PARAMS *props = (PARAMS *)arg;
	struct network_data *shared_data = &(props->ndata); // - shared data
	int sockfd = props->sockfd;							//  - client_id
	int lock_id = props->lock_id;
	//
	int readn;
	socklen_t addrlen;
	struct sockaddr_in client_addr;

	char buf[MAXLINE];
	memset(buf, 0x00, MAXLINE);
	addrlen = sizeof(client_addr);

	getpeername(sockfd, (struct sockaddr *)&client_addr, &addrlen);

	struct network_data n_data;

	printf("✔️ waiting client data... %d \n", sockfd);
	if ((readn = read(sockfd, (void *)&n_data, sizeof(n_data))) > 0)
	{
		pthread_mutex_lock(&t_lock[lock_id]); // --- CS 섹션 START

		/* produce(1) - nework io */
		int num = ntohl(n_data.num);
		char content[100];
		strcpy(content, n_data.content);
		printf("Read Data %s(%d) : %s,%d\n",
			   inet_ntoa(client_addr.sin_addr),
			   ntohs(client_addr.sin_port),
			   content,
			   num);
		shared_data->num = num;
		strcpy(shared_data->content, content);
		pthread_mutex_unlock(&t_lock[lock_id]); // --- CS 섹션 END

		/* produce(2) - write shared thread */
		for (int i = 0; i < 100; i++)
		{
			pthread_mutex_lock(&t_lock[lock_id]); // --- CS 섹션 START
			shared_data->num++;
			rotate(shared_data->content);
			pthread_cond_signal(&t_cond[lock_id]);
			pthread_mutex_unlock(&t_lock[lock_id]); // --- CS 섹션 END
			sleep(1);
		}
	}

	close(sockfd);
	printf("worker-producer thread end\n");
	return 0;
}

void *thread_func_consumer(void *arg)
{
	printf("✔️ consumer-thread construced \n");
	// args typecasting
	PARAMS *props = (PARAMS *)arg;
	struct network_data *shared_data = &(props->ndata); // - shared data
	int sockfd = props->sockfd;							//  - client_id
	int lock_id = props->lock_id;
	//
	socklen_t addrlen;
	char buf[MAXLINE];
	struct sockaddr_in client_addr;
	memset(buf, 0x00, MAXLINE);
	addrlen = sizeof(client_addr);

	getpeername(sockfd, (struct sockaddr *)&client_addr, &addrlen);

	struct network_data n_data;

	for (int i = 0; i < 100; i++)
	{
		pthread_mutex_lock(&t_lock[lock_id]);
		pthread_cond_wait(&t_cond[lock_id], &t_lock[lock_id]); // > wait 조건 변수를 사용하는 영역을 mutex로 보호

		// read from thread shared memory
		int num = shared_data->num; // ntohl(n_data.num);
		char *content = shared_data->content;
		printf("(consumer)Read shared Data  %d %s < ", num, content);
		pthread_mutex_unlock(&t_lock[lock_id]); // --- CS 섹션 END

		n_data.num = htonl(num);
		strcpy(n_data.content, shared_data->content);
		write(sockfd, (void *)&n_data, sizeof(n_data));
		// sleep(1);
	}
	close(sockfd);
	printf("worker-consumer thread end\n");
	return 0;
}

int main(int argc, char **argv)
{
	char srcs[10] = "hello";
	rotate(srcs);
	int listen_fd, client_fd;
	socklen_t addrlen;
	int readn;
	char buf[MAXLINE];

	pthread_t thread_id[THREAD_MAX_NUM];
	int thread_id_cnt = 0;

	struct sockaddr_in server_addr, client_addr;

	/* - thread initalize  */
	// 스레드간 heap 공유 heap 변수
	struct thread_data tdata;
	for (int i = 0; i < THREAD_MAX_NUM; i++)
	{

		pthread_mutex_init(&t_lock[i], NULL);
		pthread_cond_init(&t_cond[i], NULL);
	}

	/* socket create & bind & listen */
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
	/* client accpet & make thread */
	while (1)
	{
		/* [1] accept */
		addrlen = sizeof(client_addr);
		client_fd = accept(listen_fd,
						   (struct sockaddr *)&client_addr, &addrlen);
		printf("new client connected from %s:%d (%d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);
		if (client_fd == -1)
		{
			printf("accept error\n");
		}
		else
		/* [2] thread_create -  */
		{
			printf("make prod/cons thread & args\n");
			PARAMS *pMain = malloc(sizeof(PARAMS));
			// memset((void *)&pMain, 0x00, sizeof(PARAMS));
			pMain->sockfd = client_fd;
			pMain->consumer_tid = thread_id_cnt + 1;
			pMain->producer_tid = thread_id_cnt + 2;
			pMain->lock_id = lock_cnt;
			printf("client_fd  %d ~ pMain->sockfd %d  address %p\n", client_fd, pMain->sockfd, pMain);

			// thread_create(id,,thread_func_producer,args...)
			// (ㅠBUG)heap 공유할때 그냥 메모리 주소값을 넘기면 되는데, 포인터의 주소값을 넘겨버림
			pthread_create(&thread_id[pMain->consumer_tid], NULL, thread_func_producer, (void *)pMain);
			pthread_create(&thread_id[pMain->producer_tid], NULL, thread_func_consumer, (void *)pMain);

			// pthread_detach() 함수는 pthread_join()을 사용하지 않더라도, 쓰레드 종료될때 모든 자원을 해제 됩니다
			pthread_detach(thread_id[pMain->consumer_tid]);
			pthread_detach(thread_id[pMain->producer_tid]);
			thread_id_cnt += 2;
			lock_cnt++;
		}
	}
	return 0;
}

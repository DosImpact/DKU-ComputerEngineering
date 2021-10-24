#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAXLINE 1024
#define PORTNUM 3600

int main(int argc, char **argv)
{
	int listen_fd, client_fd[3] = {0, 0, 0};
	pid_t pid;
	socklen_t addrlen;
	int readn;
	char buf_list[3][MAXLINE];	  // 3개의 클라이언트에서 온 데이터를 저장.
	char buf_strcat[MAXLINE * 3]; // 3개의 클라이언트에서 온 데이터를 합친 결과를 저장
	struct sockaddr_in client_addr, server_addr;

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
		// 클라이언트의 접속을 3번 대기(-blocking) 및 데이터 입력을 받는다.
		for (int i = 0; i < 3; i++)
		{
			client_fd[i] = accept(listen_fd,
								  (struct sockaddr *)&client_addr, &addrlen);
			if (client_fd[i] == -1)
			{
				printf("accept error\n");
				break;
			}
			memset(buf_list[i], 0x00, MAXLINE);
			// 클라이언트로 데이터 입력받기
			readn = read(client_fd[i], buf_list[i], MAXLINE);
			if (readn > 0)
			{
				printf("Read Data %s(%d) : %s",
					   inet_ntoa(client_addr.sin_addr),
					   client_addr.sin_port,
					   buf_list[i]);
				strcat(buf_strcat, buf_list[i]);
			}
		}
		// 전송된 문자열에는 개행문자가 포함되어 있으므로 제거한다.
		char *ptr = strchr(buf_strcat, '\n');
		while (ptr != NULL)
		{
			*ptr = ' ';
			ptr = strchr(ptr + 1, '\n');
		}
		printf("input data complete\n");

		// for문을 사용해서, 부모 프로세스의 상태를 가지고 fork를 한다.
		// i = 0 으로 시작해 첫번재 클라이언트에게 응답 메시지를 보내고 종료하는 자식 프로세스 - 1
		// i = 1 으로 시작해 두번재 클라이언트에게 응답 메시지를 보내고 종료하는 자식 프로세스 - 2
		// i = 2 으로 시작해 세번재 클라이언트에게 응답 메시지를 보내고 종료하는 자식 프로세스 - 3 이후 종료
		for (int i = 0; i < 3; i++)
		{
			pid = fork();
			if (pid == 0) //case:child
			{
				close(listen_fd);
				write(client_fd[i], buf_strcat, strlen(buf_strcat));
				sleep(10);
				memset(buf_list[i], 0x00, MAXLINE);
				close(client_fd[i]);
				return 0;
			}
			else if (pid > 0) //case:parent
				close(client_fd[i]);
		}
	}
	return 0;
}

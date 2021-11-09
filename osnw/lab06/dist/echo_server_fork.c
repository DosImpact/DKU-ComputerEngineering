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

#define MAXLINE 1024
#define PORTNUM 3600

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
		for (int i = 0; i < 3; i++)
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

			// [1] 클라이언트로부터 문자열 받기 ( for IPC)
			memset(buf_list[0], 0x00, MAXLINE);
			readn = read(client_fd, buf_list[0], MAXLINE);
			if (readn > 0)
			{
				printf("Read Data (for IPC) %s(%d) : %s", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, buf_list[0]);
				strcat(buf_strcat, buf_list[0]);
			}

			/* (child) IPC PIPE 열기 (주의 w,r 네임드 파이프가 P,C 간에 크로스 되어야함 ) */
			rfd = open("/tmp/myfifo_w", O_RDWR);
			if (rfd < 0)
			{
				perror("read open error\n");
				return 1;
			}
			wfd = open("/tmp/myfifo_r", O_RDWR); // open named pipe myfifo_r as write buffer
			if (wfd < 0)
			{
				perror("write open error\n");
				return 1;
			}
			while (1)
			{
				// [2] 부모 프로세스에 보내기 ( IPC PIPE)
				printf("> [2] 부모 프로세스에 보내기 \n");
				write(wfd, buf_list[0], strlen(buf_list[0])); // write -> to
				// [3] 부모 프로세스로 부터 합처진 문자열 받기
				read(rfd, buf_name_pipe_list_strcat, MAXLINE); // read
				printf("✔ Data from Parent process -> %s \n", buf_name_pipe_list_strcat);
				break;
			}
			/* IPC 종료 */
			close(wfd);
			close(rfd);

			// [6] 합처진 문자열 클라이언트로 보내기
			printf("⚠ [6] 합처진 문자열 클라이언트로 보내기 \n");
			write(client_fd, buf_name_pipe_list_strcat, strlen(buf_name_pipe_list_strcat));
			sleep(1);
			memset(buf_list[0], 0x00, MAXLINE);
			/* 클라이언트 소켓 종료 */
			close(client_fd);
			return 0;
		}
		else if (pid > 0) //case:parent
		{
			printf("⚠ pipe open start...\n");
			/* make named PIPE */
			mkfifo("/tmp/myfifo_r", S_IRUSR | S_IWUSR); // make read FIFO ( pipe )
			mkfifo("/tmp/myfifo_w", S_IRUSR | S_IWUSR); // make write FIFO ( pipe )

			/* (parent) IPC PIPE 열기 (주의 w,r 네임드 파이프가 P,C 간에 크로스 되어야함 ) */
			if ((rfd = open("/tmp/myfifo_r", O_RDWR)) == -1)
			{
				perror("rfd error");
				return 0;
			}
			if ((wfd = open("/tmp/myfifo_w", O_RDWR)) == -1)
			{
				perror("wfd error");
				return 0;
			}
			printf("✔ pipe(IPC) connected\n");

			// [4] 3개의 자식 프로세스로부터 문자열 받기 (IPC)
			for (int i = 0; i < 3; i++)
			{
				memset(buf_name_pipe_list[i], 0x00, MAXLINE);
				/* try read pipe */
				if (read(rfd, buf_name_pipe_list[i], MAXLINE) < 0)
				{
					perror("Read Error");
					return 1;
				}
				printf("Read from childProcess (IPC) : %s", buf_name_pipe_list[i]);
				lseek(wfd, 0, SEEK_SET); // ? 버퍼의 읽기 위치를 처음으로

				// 문자열 합쳐놓기
				strcat(buf_name_pipe_list_strcat, buf_name_pipe_list[i]);
			}

			// IPC를 위한 과정
			// 전송된 문자열에는 개행문자가 포함되어 있으므로 제거한다.
			char *ptr = strchr(buf_name_pipe_list_strcat, '\n');
			while (ptr != NULL)
			{
				*ptr = ' ';
				ptr = strchr(ptr + 1, '\n');
			}
			printf("input data complete\n");

			// [5] 합친 문자열 자식 프로세스로 보내기
			for (int i = 0; i < 3; i++)
			{
				write(wfd, buf_name_pipe_list_strcat, MAXLINE); // ?
			}
			close(client_fd);
		}
	}
	return 0;
}

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>

#define MAXLINE 1024
#define PORTNUM 3600
#define SOCK_SETSIZE 1021

int check_sockfd[100];

struct network_data
{
	int num;
	char content[100];
};

int main(int argc, char **argv)
{
	int listen_fd, client_fd;
	socklen_t addrlen;
	int fd_num;
	int maxfd = 0;
	int sockfd;
	int i = 0;
	char buf[MAXLINE];
	fd_set readfds, allfds;

	struct sockaddr_in server_addr, client_addr;
	struct network_data agg_data;
	memset((void *)&agg_data, 0x00, sizeof(agg_data));
	agg_data.num = 0;

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket error");
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

	FD_ZERO(&readfds);
	FD_SET(listen_fd, &readfds);

	maxfd = listen_fd;

	// thread_create(id,,thread_func_producer,args...)
	// (ㅠBUG)heap 공유할때 그냥 메모리 주소값을 넘기면 되는데, 포인터의 주소값을 넘겨버림

	while (1)
	{
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		allfds = readfds;
		printf("Select Wait %d\n", maxfd);
		// select system call - if event happen wake up me
		fd_num = select(maxfd + 1, &allfds, (fd_set *)0,
						(fd_set *)0, &timeout);

		// fd_set 테이블을 검사한다.
		if (FD_ISSET(listen_fd, &allfds))
		{
			addrlen = sizeof(client_addr);
			client_fd = accept(listen_fd,
							   (struct sockaddr *)&client_addr, &addrlen);
			// fd_set에 검사할 파일 목록 추가
			FD_SET(client_fd, &readfds);

			if (client_fd > maxfd)
				maxfd = client_fd;
			printf("Accept OK\n");
			continue;
		}

		for (i = 0; i <= maxfd; i++)
		{
			sockfd = i;
			if (FD_ISSET(sockfd, &allfds))
			{
				struct network_data n_data;
				memset(buf, 0x00, MAXLINE);
				if (read(sockfd, (void *)&n_data, sizeof(n_data)) <= 0)
				{
					// 연결 해제
					close(sockfd);
					FD_CLR(sockfd, &readfds);
				}
				else
				{
					// 성공적으로 읽음
					if (strncmp(buf, "quit\n", 5) == 0)
					{
						check_sockfd[sockfd] = 0;
						close(sockfd);
						FD_CLR(sockfd, &readfds);
					}
					else
					{
						check_sockfd[sockfd] = 1;
						int num = ntohl(n_data.num);
						char content[100];
						strcpy(content, n_data.content);
						printf("Read Data %s(%d) : %s,%d\n",
							   inet_ntoa(client_addr.sin_addr),
							   ntohs(client_addr.sin_port),
							   content,
							   num);

						agg_data.num += num;
						strcat(agg_data.content, content);

						// printf("agg_data : %d \n", agg_data.num);
						// printf("Read : %s", buf);
						// write(sockfd, buf, strlen(buf));
						if (write(sockfd, (void *)&agg_data, sizeof(agg_data)) < 0)
						{
							printf("fail~\n");
						}
					}
				}
				if (--fd_num <= 0)
					break;
			}
			else
			{
				if (check_sockfd[sockfd] == 1)
				{
					if (write(sockfd, (void *)&agg_data, sizeof(agg_data)) < 0)
					{
					}
				}
			}
		}
	}
}

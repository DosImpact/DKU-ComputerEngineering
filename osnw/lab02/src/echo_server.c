#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXBUF 1024
/**
 * echo_server
 * useage: ./echo_server 3600
 */
int main(int argc, char **argv)
{
	int server_sockfd, client_sockfd;
	int client_len, n;
	char buf[MAXBUF];
	struct sockaddr_in clientaddr, serveraddr;
	client_len = sizeof(clientaddr);
	// 1. 소켓생성
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		perror("socket error : ");
		exit(0);
	}
	memset(&serveraddr, 0x00, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;				//
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //
	serveraddr.sin_port = htons(atoi(argv[1]));		// 포트 설정
	// 2. 소켓을 IP,Port를 묶는다.
	bind(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	// 3. 수신 대기열 생성(listen queue)
	listen(server_sockfd, 5);

	while (1)
	{
		// 4. 연결 대기
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr,
							   &client_len);
		printf("New Client Connected: %s\n", inet_ntoa(clientaddr.sin_addr));
		memset(buf, 0x00, MAXBUF);
		// 5. read 데이터 통신
		if ((n = read(client_sockfd, buf, MAXBUF)) <= 0)
		{
			close(client_sockfd);
			continue;
		}
		// 6. write 데이터 통신
		if (write(client_sockfd, buf, MAXBUF) <= 0)
		{
			perror("write error : ");
			close(client_sockfd);
		}
		// 7. 소켓 닫기
		close(client_sockfd);
	}
	// 7. 소켓 닫기
	close(server_sockfd);
	return 0;
}

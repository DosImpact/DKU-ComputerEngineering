#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXBUF 1024

int main(int argc, char **argv)
{
	int server_sockfd;
	int client_sockfd[3]={0,0,0};
	int client_len, n;
	char buf[MAXBUF];
	struct sockaddr_in clientaddr, serveraddr;
	client_len = sizeof(clientaddr);
	// 
	char inputs[MAXBUF] = "";
    // 1. create socket 
	if ((server_sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1)
	{
		perror("socket error : ");
		exit(0);
	}
	memset(&serveraddr, 0x00, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(atoi(argv[1]));
    // 2. bind socket
	bind (server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	// 3. listen socket
    listen(server_sockfd, 5);

	while(1)
	{
        // 4. accpet client
		// 3개의 클라이언트의 접속을 기다린다. accpet를 실행 후 소캣번호를 client_sockfd배열에 저장
		for(int i = 0 ; i < 3; i++){
			client_sockfd[i] = accept(server_sockfd, (struct sockaddr *)&clientaddr,
				&client_len);
			printf("New Client Connect: %s[%d]\n", inet_ntoa(clientaddr.sin_addr),i);
		}
		// 3개의 클라이언트의 전송을 기다리린다.
		// read로 전송된 메시지를 버퍼에 저장 후 inputs 에 strcat으로 문자열 저장
		for(int i = 0 ; i < 3; i++){
			memset(buf, 0x00, MAXBUF);
			if ((n = read(client_sockfd[i], buf, MAXBUF)) <= 0)
			{
				close(client_sockfd[i]);
				continue;
			}
			printf("incoming msg %s",buf);
			strcat(inputs,buf);
		}
		// 전송된 문자열에는 개행문자가 포함되어 있으므로 제거한다.
		char* ptr = strchr(inputs, '\n');
		while(ptr !=NULL){
			*ptr = ' ';
			ptr = strchr(ptr + 1, '\n'); 
		}
		// 3개의 클라언트에게 합쳐진 메시지를 전송한다.
		for(int i = 0 ; i < 3; i++){
			if (write(client_sockfd[i], inputs, MAXBUF) <=0)
			{
				perror("write error : ");
				close(client_sockfd[i]);
			}
		}
	}
	close(server_sockfd);
	return 0;
}

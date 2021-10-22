# Week04

## 4장 네트워크 프로그램 개발

소켓 : 전화기의 역할을 한다, 인터넷과의 접점 혹은 관문

1. 송신자 Node를 인터넷에 연결하기

- 인터넷에 식별가능한 IP주소를 소켓에 부여
- 포트번호를 bind 한다.

2. 수신자 Node를 찾기

- 수신자 Node도 IP+PORT 를 가지고 있으므로
- IP주소로 컴퓨터의 위치를 찾고
- PORT 번호로 프로그램을 찾는다.

### 코드

- echo_server.c

```c++
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
```

- echo.client.c

```c++
#include <sys/socket.h> /* 소켓 관련 함수 */
#include <arpa/inet.h>  /* 소켓 지원을 위한 각종 함수 */
#include <sys/stat.h>
#include <stdio.h>  /* 표준 입출력 관련 */
#include <string.h> /* 문자열 관련 */
#include <unistd.h> /* 각종 시스템 함수 */

#define MAXLINE 1024

int main(int argc, char **argv)
{
    struct sockaddr_in serveraddr;
    int server_sockfd;
    int client_len;
    char buf[MAXLINE];
    // 1. 소켓 생성
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("error :");
        return 1;
    }

    /* 연결요청할 서버의 주소와 포트번호 프로토콜등을 지정한다. */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(3600);

    client_len = sizeof(serveraddr);
    // 2. 서버에 연결을 시도
    if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len) == -1)
    {
        perror("connect error :");
        return 1;
    }

    memset(buf, 0x00, MAXLINE);
    // 3. 데이터 통신 read/write
    read(0, buf, MAXLINE);                       /* 키보드 입력을 기다린다. */
    if (write(server_sockfd, buf, MAXLINE) <= 0) /* 입력 받은 데이터를 서버로 전송한다. */
    {
        perror("write error : ");
        return 1;
    }
    memset(buf, 0x00, MAXLINE);
    /* 서버로 부터 데이터를 읽는다. */
    if (read(server_sockfd, buf, MAXLINE) <= 0)
    {
        perror("read error : ");
        return 1;
    }
    printf("read : %s", buf);
    // 4. 연결 종료
    close(server_sockfd);
    return 0;
}

```

### 라인 설명

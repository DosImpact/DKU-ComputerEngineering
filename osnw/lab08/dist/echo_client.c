#include <sys/socket.h> /* 소켓 관련 함수 */
#include <arpa/inet.h>  /* 소켓 지원을 위한 각종 함수 */
#include <sys/stat.h>
#include <stdio.h>  /* 표준 입출력 관련 */
#include <string.h> /* 문자열 관련 */
#include <unistd.h> /* 각종 시스템 함수 */
#include <stdlib.h>

#define MAXLINE 1024
struct network_data
{
    int num;
    char content[100];
};

int main(int argc, char **argv)
{
    struct sockaddr_in serveraddr;
    int server_sockfd;
    int client_len;
    char buf[MAXLINE];

    struct network_data n_data;

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

    memset((void *)&n_data, 0x00, sizeof(n_data)); // reset & fill
    // n_data.num = atoi(argv[1]);

    // 3. 데이터 통신 read/write
    memset(buf, 0x00, MAXLINE);
    printf("input string : \n");
    read(0, buf, MAXLINE); /* 키보드 입력을 기다린다. - 1 */
    strcpy(n_data.content, buf);

    memset(buf, 0x00, MAXLINE);
    printf("input integer : \n");
    read(0, buf, MAXLINE); /* 키보드 입력을 기다린다. - 2*/
    n_data.num = atoi(buf);

    /* n_data.num  */
    n_data.num = htonl(n_data.num);
    if (write(server_sockfd, (void *)&n_data, sizeof(n_data)) <= 0) /* 입력 받은 데이터를 서버로 전송한다. */
    {
        perror("write error : ");
        return 1;
    }
    /* 서버로 부터 데이터를 읽는다. */
    while (1)
    {
        if (read(server_sockfd, (void *)&n_data, sizeof(n_data)) <= 0)
        {
            perror("read error : ");
            return 1;
        }
        n_data.num = ntohl(n_data.num);
        printf("read : %d \n", n_data.num);
    }
    // 4. 연결 종료
    close(server_sockfd);
    return 0;
}

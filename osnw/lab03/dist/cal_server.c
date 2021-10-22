#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 3700
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

// 클라이언트와 주고받을 구조체를 정의한다.
struct cal_statistics_data
{
        int num;
        int result_min;
        int result_max;
        int result_avg;
        short int error;
};

int main(int argc, char **argv)
{
        struct sockaddr_in client_addr, sock_addr;
        int listen_sockfd;
        int client_sockfd[3];
        int addr_len;
        // 3개의 클라이언틔에 받은 데이터를 저장하기위해 배열을 선언
        struct cal_statistics_data rdata[3];
        int num[3] = {0, 0, 0};
        int result_min = 0, result_max = 0, result_avg = 0;
        short int cal_error;
        // 소켓  생성
        if ((listen_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Error ");
                return 1;
        }

        memset((void *)&sock_addr, 0x00, sizeof(sock_addr));
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        sock_addr.sin_port = htons(PORT);
        // 바인드 & 리슨
        if (bind(listen_sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1)
        {
                perror("Error ");
                return 1;
        }

        if (listen(listen_sockfd, 5) == -1)
        {
                perror("Error ");
                return 1;
        }

        for (int i = 0; i < 3; i++)
        {
                addr_len = sizeof(client_addr);
                // 클라이언트 접속 대기 (blocking)
                client_sockfd[i] = accept(listen_sockfd,
                                          (struct sockaddr *)&client_addr, &addr_len);
                if (client_sockfd[i] == -1)
                {
                        perror("Error ");
                        return 1;
                }
                printf("New Client Connect : %s\n", inet_ntoa(client_addr.sin_addr));
                // 클라이언트의 데이터 읽기 - 읽은 후 i번에 저장한다.
                read(client_sockfd[i], (void *)&rdata[i], sizeof(rdata[i]));
                cal_error = 0;
                // network to host (long type) 으로 바이트오더를 처리
                num[i] = ntohl(rdata[i].num);
        }
        // 평균,최소,최대 값을 구한다.
        result_min = num[0];
        result_max = num[0];
        result_avg = (num[0] + num[1] + num[2]) / 3;
        for (int i = 0; i < 3; i++)
        {
                result_max = max(result_max, num[i]);
                result_min = min(result_min, num[i]);
        }
        // 각 클라이언트로, 결과값을 보내준다.
        for (int i = 0; i < 3; i++)
        {
                // host  to network  (long type | short type ) 으로 바이트오더를 처리
                rdata[i].result_min = htonl(result_min);
                rdata[i].result_max = htonl(result_max);
                rdata[i].result_avg = htonl(result_avg);
                rdata[i].error = htons(cal_error);
                write(client_sockfd[i], (void *)&rdata[i], sizeof(rdata[i]));
                close(client_sockfd[i]);
        }
        close(listen_sockfd);
        return 0;
}

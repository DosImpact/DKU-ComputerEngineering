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
#define IP "127.0.0.1"

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
    struct sockaddr_in addr;
    int s;
    int len;
    int sbyte, rbyte;
    struct cal_statistics_data sdata;
    if (argc != 2)
    {
        printf("Usage : %s [num1]\n", argv[0]);
        return 1;
    }

    memset((void *)&sdata, 0x00, sizeof(sdata));
    sdata.num = atoi(argv[1]);
    // sdata.right_num = atoi(argv[2]);
    // sdata  argv[3][0];

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == -1)
    {
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("fail to connect\n");
        close(s);
        return 1;
    }

    len = sizeof(sdata);
    sdata.num = htonl(sdata.num);
    // 데이터를 서버로 보낸다.
    sbyte = write(s, (char *)&sdata, len);
    if (sbyte != len)
    {
        return 1;
    }
    // 데이터를 클라이언트로 받는다.
    rbyte = read(s, (char *)&sdata, len);
    if (rbyte != len)
    {
        return 1;
    }
    //network to host (long type | short type ) 으로 바이트오더를 처리
    if (ntohs(sdata.error != 0))
    {
        printf("CALC Error %d\n", ntohs(sdata.error));
    }
    printf("your input : %d avg: %d max: %d min: %d\n",
           ntohl(sdata.num), ntohl(sdata.result_avg), ntohl(sdata.result_max), ntohl(sdata.result_min));

    close(s);
    return 0;
}

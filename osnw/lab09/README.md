## 개요

IO 멀티플랙싱을 이용해서, 소켓프로그래밍을 하자.

## 목표

과제 시나리오

1. IO 멀티플랙싱 서버를 만든다.  
   클라이언트의 최초 접속인 경우 - 연결소켓으로 처리한다.  
   클라이언트의 재접속인 경우 - 클라이언트 소켓으로 처리한다. read/write

   client1 에서 os 100을 입력 받는다.  
   -> 연결된 모든 클라이언트에게 os 100 을 답장  
   client2 에서 nw 200을 입력 받는다.  
   -> 연결된 모든 클라이언트에게 osnw 300 을 답장  
   client3 에서 system 300을 입력 받는다.  
   -> 연결된 모든 클라이언트에게 osnw system 600 을 답장

2. 클라이언트는 데이터를 받는 즉시, print로 출력을 한다.

### 핵심 키워드

IO 멀티플랙싱, select system call

### 핵심 로직

1. select system call에서 1초 동안 이벤트를 대기 한다.
2. 이벤트가 있는 경우 다음으로 나누어서 처리

- 연결소켓(accpet) - 클라이언트의 접속을 기록
- 클라이언트 소켓(read/write)을 처리한다.
  새로운 클라이언트 접속시, check 배열에 추가

3. check배열(현재 연결된 클라이언트)에게 누산된 데이터를 보낸다.

## dist 코드 컴파일

gcc ./echo_client_loop.c -o echo_client_loop -lpthread
gcc ./echo_server_multi.c -o echo_server_multi

```c++

struct network_data
{
    int num;
    char content[100];
};

struct network_data n_data;


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
    printf("read from server : %d %s ", n_data.num, n_data.content);
}
// 4. 연결 종료
close(server_sockfd);

```

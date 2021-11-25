## 목표

과제 시나리오

1. server는 client 연결 후 구조체(string,정수) 를 받는다.
2. fork 로 클라이언트를 응대하는 child 멀티 프로세스를 만든다.
3. 컨슈머를 만들어서(fork) 무한히 string(rotating),정수+1 하여 공유 메모리에 쓴다.  
   이때 세마포어를 사용해서 CS 를 보호할 것
4. 프로듀서는 공유메모리의 데이터를 읽어서 클라이언트로 데이터를 반드다.
5. client 프로세스는 데이터가 오는 즉시, echo 를 한다.

공유 메모리로 server의 parentd-child process간 소통이 되어야 한다.

- 세마포어를 사용할 것

### 핵심 키워드

멀티프로세스, fork
소켓프로그래밍
생성자 소비자
쉐어드 메모리
세마포어

### IPCS 확인 및 삭제 명령어

- ipcs 확인하기

```
ubuntu@ubuntu:~/workspace$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x000004d2 0          ubuntu     666        4          2

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
0x00000d95 0          ubuntu     666        1
```

- ipcs 삭제하기

```
ipcrm shm 1234(공유메모리 id)
ipcrm sem 3477(사마포어 id)


ubuntu@ubuntu:~/workspace$ ipcrm shm 0
resource(s) deleted
ubuntu@ubuntu:~/workspace$ ipcrm sem 0
resource(s) deleted


ubuntu@ubuntu:~/workspace$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status

------ Semaphore Arrays --------
key        semid      owner      perms      nsems

```

## 예제 코드 실행법

gcc shm_consumer_sem.c -o shm_consumer_sem
gcc shm_producer_sem.c -o shm_producer_sem

## dist 코드 컴파일

gcc ./echo_server_fork.c -o echo_server_fork
gcc ./echo_client.c -o echo_client

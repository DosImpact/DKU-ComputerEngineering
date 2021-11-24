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

## dist 코드 컴파일

gcc ./echo_server_fork.c -o echo_server_fork
gcc ./echo_client.c -o echo_client

## 예제 코드(src폴더) 설명

### 스레드 라이브러리 포함하는 옵션을 주어 컴파일 하기

gcc 컴파일시 lpthread 라는 라이브러리를 링크할때 써라

> gcc -o thread1 thread1.c -lpthread

프로세스의 상태를 볼 때 L 옵션을 주어 스레드까지 보자.

> ps -eLf

### thread1.c

```
/* 스레드 예제1) 생성  */
```

### thread2.c

```

/* 스레드 예제2) 생성 (+인자값)  */
// 문제점 : main 스레드의 stack 변수를 참조하므로, 스레드간 의존성이 높아졌다.
```

### thread3.c

```
/* 스레드 예제3) 생성 (+인자값)  */
// 문제점 : main 스레드의 stack 변수를 참조하므로, 스레드간 의존성이 높아졌다.
// thread2예제와 같은 로직이지만 sub함수로 분리를 하였더니, 변수참조에 문제가 발생
// (워커스레드에서 사용중인 변수를 메인스레드에서 sub함수 종료 후 변수를 제거 한 상황  )
```

### thread4.c

```
/* 스레드 예제4) 생성 (+인자값)  */
// 워커스레드에서 사용중인 동적할당 변수를 메인스레드에서 free(해체) 한 상황
// *스레드간 heap영역을 공유하므로 서로간의 합이 안맞은 상황이다.
```

### count_thread.c

```
/* 스레드 예제5) 스레드 카운터

  메인 스레드의 count 변수를 , 워커 스레드들이 참조하여 +1 씩 올려주고 있다.
  이 예제에서는 race condition문제가 발생하지 않는다.
*/
```

### count_thread_race.c

```
/* 스레드 예제6) 스레드 카운터  (+레이스 컨디션)

  메인 스레드의 count 변수를 , 워커 스레드들이 참조하여 +1 씩 올려주고 있다.
  이 예제에서는 race condition문제가 발생한다.
  2개의 워커 스레드가 count변수에 동시에 접근하게 된다.
  이를 다음 예제에서는 한 스레드만 CS에 접근하도록 mutex를 걸자
	...
	281473203630560 7
	281473195237856 7
	Main Thread : 8
	...
*/
```

### count_thread_mutex.c

```
/* 스레드 예제 7) 스레드 카운터  (+레이스 컨디션,+ mutex lock)

  메인 스레드의 count 변수를 , 워커 스레드들이 참조하여 +1 씩 올려주고 있다.
  mutex lock 을 통해 한번에 한 스레드만 count 변수에 접근이 가능하다.
*/
```

### calc_multi.c

```
/* 스레드 예제 8) 스레드간 동기화 ( signal & wait )

*/
```

### echo_server_thread.c

```
/* 스레드 예제 9) 스레드 에코 서버
	- 서버 접속시 accpet은 메인 스레드가 ,
	- 이후 read/write는 서브 스레드가 담당
*/
```

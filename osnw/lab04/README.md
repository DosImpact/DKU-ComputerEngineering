## 목표

UDP 통신으로 , 3개의 클라이언트로부터 문자열을 받아서
합친 문자열을 리턴하도록 한다.

## src 실행

1. UDP 통신으로 , 사칙연산을 하는 통신 프로그램을 만들자

```
gcc cal_linux_cli.c -o cal_linux_cli && gcc cal_linux_server.c -o cal_linux_server

# 서버 실행
./cal_linux_server
# 클라이언트 접속
./cal_linux_cli 127.0.0.1

> 1+2
> 1 + 2 = 3
```

2. 포크 테스트

```
gcc ./fork_test.c -o fork_test
./fork_test

# 프로세스 확인
# - PID 193596 프로세스 하나만 있었는데, fork 이후 두개의 프로세스로 확장되었다.
# - PID 193606 프로세스의 부모는 193596임을 확인할 수 있다.

ubuntu@ubuntu:~/workspace$ ps -ef | grep fork_test
ubuntu    193596(*내 PID)  193441  0 11:03 pts/22   00:00:00 ./fork_test
ubuntu    193605  193597  0 11:03 pts/23   00:00:00 grep --color=auto fork_test
ubuntu@ubuntu:~/workspace$ ps -ef | grep fork_test
ubuntu    193596  193441  0 11:03 pts/22   00:00:00 ./fork_test
ubuntu    193606(*내 PID)  193596(*부모 PID)  0 11:03 pts/22   00:00:00 ./fork_test
ubuntu    193608  193597  0 11:03 pts/23   00:00:00 grep --color=auto fork_test

```

3. 포크 마이쉘

```
gcc ./myshell.c -o myshell
./myshell

# child로 fork 해서 쉘을 실행시키고 종료
My Shell Ver 1.0
> ls
Child wait
Execl failure
Child exit
```

4. 데몬 프로세스 child_wait.c

```
gcc ./child_wait.c -o child_wait
./child_wait

# defunct 라는 좀비프로세스를 확인할 수 있다.
ubuntu@ubuntu:~/workspace$ ps -ef | grep child_wait
ubuntu    193773  193441  0 11:07 pts/22   00:00:00 ./child_wait
ubuntu    193774  193773  0 11:07 pts/22   00:00:00 [child_wait] <defunct>
ubuntu    193784  193597  0 11:07 pts/23   00:00:00 grep --color=auto child_wait
```

5. 멀티 프로세스 에코 서버 echo_client.c, echo_server_fork.c

```
gcc ./echo_client.c -o echo_client && gcc ./echo_server_fork.c -o echo_server_fork
./echo_server_fork
./echo_client


# cf - 프로세스 비정상 종료 제거
lsof -i TCP:3600
```

6. ? TODO command.c , exec_test.c

7. ? TODO daemonOSNW.c

## task 과제

UDP 통신을 이용해서 서버, 클라이언트1,2,3 를 각 프로세스로 실행시켜서 다음을 수행하자

1. 서버를 3600 포트에서 열어둔다.
2. 클라리언트1,2,3이 서버로 접속
3. 각 클라이언트에서 다음의 메시지를 각자 보낸다: (osnw2021), (학번), (이름)
4. 서버는 수신이 완료되면, 합처진 (osnw2021) (학번) (이름) 문자열을 클라이언트로 각각 전송한다.

- 단, 클라이언트로 보낼땐 fork 를 통해서 자식 프로세스가 보내도록 해야한다.

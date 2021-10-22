## 목표

서버, 클라이언트1,2,3 를 각 프로세스로 실행시켜서 다음을 수행하자

1. 서버를 3600 포트에서 열어둔다.
2. 클라리언트1,2,3이 서버로 접속
3. 각 클라이언트에서 다음의 메시지를 각자 보낸다: (osnw2021), (학번), (이름)
4. 서버는 수신이 완료되면, 합처진 (osnw2021) (학번) (이름) 문자열을 클라이언트로 각각 전송한다.

## src 실행

rm ./echo_client && rm ./echo_server
gcc echo_client.c -o echo_client && gcc echo_server.c -o echo_server

./echo_server 3600
./echo_client

hello
read : hello

## task 과제

클라이언트 3개의 접속을 각 배열에 담는다.

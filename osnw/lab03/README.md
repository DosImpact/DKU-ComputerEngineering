## 목표

서버, 클라이언트1,2,3 를 각 프로세스로 실행시켜서 다음을 수행하자

1. 서버를 3700 포트에서 열어둔다.
2. 클라리언트1,2,3이 서버로 접속
3. 각 클라이언트에서 다음의 메시지를 각자 보낸다: 1,10,30
4. 서버는 수신이 완료되면, 평균, 최대값, 최소값을 클라이언트로 각각 전송한다.

## src 실행

rm ./cal_client && rm ./cal_server
gcc cal_client.c -o cal_client && gcc cal_server.c -o cal_server

./cal_server
./cal_client

hello
read : hello

## task 과제

클라이언트 3개의 접속을 각 배열에 담는다.

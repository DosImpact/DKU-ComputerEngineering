// trasfer pointer parameter of struct to thread function
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* 스레드 예제2) 생성 (+인자값)  */
// 문제점 : main 스레드의 stack 변수를 참조하므로, 스레드간 의존성이 높아졌다.

// 스레드간 데이터 공유를 위한 자료구조 정의
typedef struct
{
   char field1[10];
   char field2[10];
   int field3;
} PARAMS;

// 프로듀서 함수(스레드)
// 1. void* 을 arg로 받는다.
// 2. PARAMS* 로 타입 케스팅을 한다.
void *Producer(void *arg)
{
   PARAMS *pProducer = (PARAMS *)arg;
   sleep(1);
   printf("Producer => %s %d\n", pProducer->field1, pProducer->field3);
}
void *Consumer(void *arg)
{
   PARAMS *pConsumer = (PARAMS *)arg;
   sleep(2);
   printf("Consumer => %s %d\n", pConsumer->field2, pConsumer->field3);
}

pthread_t ThreadVector[2]; // non-local variables

void main()
{
   PARAMS pSub;

   strcpy(pSub.field1, "hello"); // set string
   strcpy(pSub.field2, "world"); // set string
   pSub.field3 = 2020;           // set int

   // pthread_create : (thread의 id, ,스레드로 실행할 함수 포인터,함수 인자.)
   pthread_create(&ThreadVector[0], NULL, Producer, (void *)&pSub);
   pthread_create(&ThreadVector[1], NULL, Consumer, (void *)&pSub);

   // 스레드 종료 대기
   pthread_join(ThreadVector[0], NULL);
   pthread_join(ThreadVector[1], NULL);
}

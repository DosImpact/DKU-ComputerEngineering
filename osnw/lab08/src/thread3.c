// 포인터가 가르키는 데이터의 유효성 문제 1 – auto variables
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* 스레드 예제3) 생성 (+인자값)  */
// 문제점 : main 스레드의 stack 변수를 참조하므로, 스레드간 의존성이 높아졌다.
// thread2예제와 같은 로직이지만 sub함수로 분리를 하였더니, 변수참조에 문제가 발생
// (워커스레드에서 사용중인 변수를 메인스레드에서 sub함수 종료 후 변수를 제거 한 상황  )

typedef struct
{
   char field1[10];
   char field2[10];
   int field3;
} PARAMS;

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

void sub()
{
   PARAMS pSub;

   strcpy(pSub.field1, "hello");
   strcpy(pSub.field2, "world");
   pSub.field3 = 2020;

   pthread_create(&ThreadVector[0], NULL, Producer, (void *)&pSub);
   pthread_create(&ThreadVector[1], NULL, Consumer, (void *)&pSub);
}
void main()
{
   sub();
   pthread_join(ThreadVector[0], NULL);
   pthread_join(ThreadVector[1], NULL);
}

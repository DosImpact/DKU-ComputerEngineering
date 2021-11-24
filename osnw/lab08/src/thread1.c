#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/* 스레드 예제1) 생성  */
void *Producer(void *arg)
{
   int i;

   for (i = 10; i < 20; i++)
      printf("Producer => %d\n", i);
}

void *Consumer(void *arg)
{
   int i;

   for (i = 20; i < 30; i++)
      printf("Consumer => %d\n", i);
}

void main()
{
   int i;
   pthread_t ThreadVector[2]; // thread의 id를 저장

   // pthread_create : (thread의 id, ,스레드로 실행할 함수 포인터,)
   pthread_create(&ThreadVector[0], NULL, Producer, NULL);
   pthread_create(&ThreadVector[1], NULL, Consumer, NULL);

   for (i = 0; i < 10; i++)
      printf("Main => %d\n", i);

   // pthread_join : 스레드 종료를 기다린다.
   pthread_join(ThreadVector[0], NULL);
   pthread_join(ThreadVector[1], NULL);
}

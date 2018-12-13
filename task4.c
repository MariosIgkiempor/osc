#include <pthread.h>
#include <semaphore.h>
#include "coursework.h"
#define MAX_NUMBER_OF_JOBS 50
#define MAX_BUFFER_SIZE 10

void *consumer(void *queue);
void *producer(void *queue);
int getLowestIndex(struct queue *q);

struct queue q;

int producedCount = 0, consumedCount = 0;
long int turnaroundTime = 0, responseTime = 0;

// initialise the two counting semaphores and the lock semaphore
sem_t sFull, sEmpty, sLock;

int main()
{
  // initialise the queue
  if (init(&q, MAX_BUFFER_SIZE) != 0)
  {
    printf("Error initialising queue.");
    return 1;
  }

  // initialise the semaphores
  sem_init(&sEmpty, 0, MAX_BUFFER_SIZE);
  sem_init(&sFull, 0, 0);
  sem_init(&sLock, 0, 1);

  // initialise the thread
  pthread_t producerThread, consumerThread;

  if (pthread_create(&producerThread, NULL, &producer, &q) != 0)
  {
    printf("Error creating producer thread\n");
    return 1;
  }

  if (pthread_create(&consumerThread, NULL, &consumer, &q) != 0)
  {
    printf("Error creating consumer thread\n");
    return 1;
  }
  pthread_join(producerThread, NULL);
  pthread_join(consumerThread, NULL);

  printf("Average response time: %.2f\n", (double)responseTime/MAX_NUMBER_OF_JOBS);
  printf("Average turnaround time: %.2f\n", (double)turnaroundTime/MAX_NUMBER_OF_JOBS);
  return 0;
}

void *consumer(void *queue)
{
  struct queue *q = (struct queue *)queue;
  while (1)
  {
    if (consumedCount == MAX_NUMBER_OF_JOBS) break;
    // struct queue tempQ;
    // init(&tempQ, 1);
    // wait for the queue to have at least 1 element
    sem_wait(&sFull);
    
    // remove a process, put it in the temp queue so it can run
    // outside of the mutex
    sem_wait(&sLock);
    struct element toRun = q->e[q->count-1];
    removeLast(q);
    printf("C: Jobs in buffer: %d, Jobs produced: %d, Jobs consumed: %d\n", q->count, producedCount, ++consumedCount);
    sem_post(&sLock);

    sem_post(&sEmpty);
    struct timeval start, end, created = toRun.created_time;
    gettimeofday(&start, NULL);
    runNonPreemptiveJobv2(&toRun);
    gettimeofday(&end, NULL);
    responseTime   += getDifferenceInMilliSeconds(created, start);
    turnaroundTime += getDifferenceInMilliSeconds(created, end); 
  }

  return NULL;
}

void *producer(void *queue)
{
  struct queue *q = (struct queue *)queue;
  while (1)
  {
    if (producedCount == MAX_NUMBER_OF_JOBS) break;

    // create a process 
    struct element e = generateProcess(); 

    // wait for room to become available
    sem_wait(&sEmpty);

    // critical section: add the element to the sorted index of the array
    sem_wait(&sLock);
    // find sorted index
    int sortedIndex = 0;
    for (int i = 0; i < q->count; i++)
    {
      if (q->e[i].pid_time > e.pid_time) sortedIndex++;
      else break;
    }
    addHere(q, &e, sortedIndex);
    printf("P: Jobs in buffer: %d, Jobs produced: %d, Jobs consumed: %d\n", q->count, ++producedCount, consumedCount);
    printAll(q);
    sem_post(&sLock);

    // decrement room available
    sem_post(&sFull);
  }
}

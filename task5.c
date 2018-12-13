#include <pthread.h>
#include <semaphore.h>
#include "coursework.h"
#define MAX_NUMBER_OF_JOBS 15
#define MAX_BUFFER_SIZE 9

int producedCount = 0, consumedCount = 0;

sem_t sEmpty, sFull, sLock;

struct queue q0, q1, q2;
struct queues_struct {
  struct queue *q0;
  struct queue *q1;
  struct queue *q2;
};

struct timeval *startTimes;
long int turnaroundTime, responseTime;

void *producer(void *queues)
{
  struct queues_struct *qs = (struct queues_struct *)queues;
  struct queue *q0 = qs->q0;
  struct queue *q1 = qs->q1;
  struct queue *q2 = qs->q2;
  struct queue *q;
  while (1)
  {
    if (producedCount == MAX_NUMBER_OF_JOBS) break;

    // create a process 
    struct element e = generateProcess(); 
    printf("Job with Priority %d\n", e.pid_priority);
    
    // find the right queue to put the new element in
    if (e.pid_priority == 0)
    {
      q = q0;
    }
    else if (e.pid_priority == 1)
    {
      q = q1;
    }
    else if (e.pid_priority == 2)
    {
      q = q2;
    }
    // wait for room to become available
    sem_wait(&sEmpty);

    // critical section: add the element to the sorted index of the array
    sem_wait(&sLock);
    // find sorted index and add the element to that index
    int sortedIndex = 0;
    for (int i = 0; i < q->count; i++)
    {
      if (q->e[i].pid_time > e.pid_time) sortedIndex++;
      else break;
    }
    addHere(q, &e, sortedIndex);
    printf("P: Jobs in buffer 0: %d, Jobs produced: %d\n", q->count, ++producedCount);
    //printAll(q0);
    sem_post(&sLock);

    // decrement room available
    sem_post(&sFull);
    }

  printf("--------------------------PRODUCER EXITED--------------------\n");
  return NULL;
}

void* consumer(void *queues)
{
  struct queues_struct *qs = (struct queues_struct *)queues;
  struct queue *q0 = qs->q0;
  struct queue *q1 = qs->q1;
  struct queue *q2 = qs->q2;
  struct queue *q;
  while (1)
  {
    if (consumedCount == MAX_NUMBER_OF_JOBS) break;
    int count = 0;
    
    // get the highest priority queue with at least one job to run
    sem_wait(&sFull);
    sem_wait(&sLock);
    if (q0->count > 0)
    {
      q = q0;
    }
    else if (q1->count > 0)
    {
      q = q1;
    }
    else if (q2->count > 0)
    {
      q = q2;
    }
    sem_post(&sLock);
    sem_post(&sFull);
    
    sem_wait(&sFull);
    sem_wait(&sLock);
    count = q->count;
    struct element toRun = q->e[count-1];
    removeLast(q);
    count = q->count;
    
    sem_post(&sLock);
    sem_post(&sEmpty);
    
    printf("C %d: Jobs in buffer: %d, Jobs produced: %d, Jobs consumed: %d\n", 0, count, producedCount, consumedCount);
    
    // run element
    //struct timeval start, end, created = toRun.created_time;
    //gettimeofday(&start, NULL);
    if (startTimes[toRun.pid].tv_sec == 0)
    {
      gettimeofday(&startTimes[toRun.pid], NULL);
    }
    runPreemptiveJobv2(&toRun);

    if (toRun.pid_time > 0) {
      sem_wait(&sEmpty);
      sem_wait(&sLock);
      addFirst(q, &toRun);
      printAll(q);
      count = q->count;
      sem_post(&sLock);
      sem_post(&sFull);
      
      printf("C %d: Re-added; Jobs in buffer: %d, Jobs produced: %d, Jobs consumed: %d\n", 0, count, producedCount, consumedCount);
    }
    else if (toRun.pid_time == 0) {
      printf("finished job %d*******************\n", toRun.pid);
      ++consumedCount;
      printf("%d consumed count ************************\n", consumedCount);
    }
    //gettimeofday(&end, NULL);
    //responseTime   += getDifferenceInMilliSeconds(created, start);
    //turnaroundTime += getDifferenceInMilliSeconds(created, end); 
  }
  
  printf("--------------------------CONSUMER 0 EXITED--------------------\n");
  return NULL;
}

int main()
{
  // inistialise the three buffer queues
  if (init(&q0, MAX_BUFFER_SIZE / 3) != 0)
  {
    printf("Error initialising queue 0.");
    return 1;
  }
  if (init(&q1, MAX_BUFFER_SIZE / 3) != 0)
  {
    printf("Error initialising queue 1.");
    return 1;
  }
  if (init(&q2, MAX_BUFFER_SIZE / 3) != 0)
  {
    printf("Error initialising queue 2.");
    return 1;
  }
  
  // initialise array of start times
  startTimes = calloc(MAX_NUMBER_OF_JOBS, sizeof(struct timeval));
  
  // initialise the semaphores
  sem_init(&sEmpty, 0, MAX_BUFFER_SIZE); 
  sem_init(&sFull, 0, 0);
  sem_init(&sLock, 0, 1);

  // create struct to pass to the producer
  struct queues_struct queues;
  queues.q0 = &q0;
  queues.q1 = &q1;
  queues.q2 = &q2;
  
  // create and initialise the producer and consumers threads
  pthread_t tProducer, tConsumer0, tConsumer1, tConsumer2;
  if (pthread_create(&tProducer, NULL, &producer, &queues) != 0)
  {
    printf("Error creating producer thread\n");
    return 1;
  }
  
  if (pthread_create(&tConsumer0, NULL, &consumer, &queues) != 0)
  {
    printf("Error creating consumer 0 thread\n");
    return 1;
  }
  
  if (pthread_create(&tConsumer1, NULL, &consumer, &queues) != 0)
  {
    printf("Error creating consumer 1 thread\n");
    return 1;
  }
  
  if (pthread_create(&tConsumer2, NULL, &consumer, &queues) != 0)
  {
    printf("Error creating consumer 2 thread\n");
    return 1;
  }
  
  // wait for all threads to finish
  pthread_join(tProducer,  NULL);
  pthread_join(tConsumer0, NULL);
  
  pthread_join(tConsumer1, NULL);
  pthread_join(tConsumer2, NULL);
  
  
  return 0;
}

#include "coursework.h"
#define MAX_PROCESSES 5

void generatePQ(struct queue *q0, struct queue *q1, struct queue *q2);
void runPQ(struct queue *q0, struct queue *q1, struct queue *q2);
void insertSorted(struct queue *q, struct element e);
void runSingleQueue(struct queue *q, int i);

long int totalTurnaroundTime = 0;
long int totalResponseTime   = 0;

int main()
{
  // Initialise and print all the queues
  struct queue q0 = (struct queue){0};
  struct queue q1 = (struct queue){0};
  struct queue q2 = (struct queue){0};
  init(&q0, MAX_PROCESSES);
  init(&q1, MAX_PROCESSES);
  init(&q2, MAX_PROCESSES);
  printf("Q #0\n");
  printAll(&q0);
  printf("Q #1\n");
  printAll(&q1);
  printf("Q #2\n");
  printAll(&q2);

  generatePQ(&q0, &q1, &q2);
  runPQ(&q0, &q1, &q2);

  double averageResponseTime   = (double)totalResponseTime   / MAX_PROCESSES;
  double averageTurnaroundTime = (double)totalTurnaroundTime / MAX_PROCESSES;
  printf("Average response time: %.2f milliseconds\n",    averageResponseTime);
  printf("Average turn around time: %.2f milliseconds\n", averageTurnaroundTime);

  freeAll(&q0);
  freeAll(&q1);
  freeAll(&q2);
}

void generatePQ(struct queue *q0, struct queue *q1, struct queue *q2)
{
  // generate MAX_PROCESSES random processes and put them in the right queue according to their priority level
  printf("Generating processes for PQ ...\n");
  for (int i = 0; i < MAX_PROCESSES; i++)
  {
    struct element e = generateProcess();
    printf("New process has priority %d\n", e.pid_priority);
    if (e.pid_priority == 0)
    {
      printf("PQ 0: add new\n");
      insertSorted(q0, e);
    }
    else if (e.pid_priority == 1)
    {
      printf("PQ 1: add new\n");
      insertSorted(q1, e);
    }
    else if (e.pid_priority == 2)
    {
      printf("PQ 2: add new\n");
      insertSorted(q2, e);
    }

    // print all the arrays
    printf("Q #0\n");
    printAll(q0);
    printf("Q #1\n");
    printAll(q1);
    printf("Q #2\n");
    printAll(q2);
  }
}

void insertSorted(struct queue *q, struct element e)
{
  int sortedIndex = 0;
  for (int j = 0; j < q->count; j++)
  {
    if (q->e[j].pid_time > e.pid_time)
    {
      sortedIndex++;
    }
  }

  // add the process to the queue at its sorted index
  addHere(q, &e, sortedIndex);
}

void runSingleQueue(struct queue *q, int i)
{
  struct timeval *startTimes = calloc(q->count, sizeof(struct timeval));
  int index = 0;
  
  while (1)
  {
    if (q->count < 1)
    { break; }
    // reset index to 0 if it points past the last element in the queue
    if (index >= q->count)
      index = 0;

    if (q->e[index].pid_time > 0)
    {
      // if the job hasn't started, add its start time to the array of start times
      if (startTimes[index].tv_sec == 0)
      {
        struct timeval startTime = (struct timeval) {0};
        gettimeofday(&startTime, NULL);
        startTimes[index] = startTime;
        int response = getDifferenceInMilliSeconds(q->e[index].created_time, startTimes[index]);
        totalResponseTime += response;

        printf("Q: %d P: %d C: %ld S: %ld R: %ld\n", i, q->e[index].pid, q->e[index].created_time.tv_sec, startTimes[index].tv_sec, response);
      }

      // run the job
      runPreemptiveJob(q, index);

      // if the job is finished, add its turnaround time to the total
      if (q->e[index].pid_time == 0)
      {
        struct timeval endTime = (struct timeval){0};
        gettimeofday(&endTime, NULL);
        long turnaround = getDifferenceInMilliSeconds(q->e[index].created_time, endTime);
        totalTurnaroundTime += turnaround;
        printf("Q: %d P: %d C: %ld E: %ld T: %ld\n", i, q->e[index].pid, q->e[index].created_time.tv_sec, endTime.tv_sec, turnaround);
      }
    }
    index++;

    // check if all processes in the queue have finished
    int allFinished = 1;
    for (int i = 0; i < q->count; i++)
    {
      if (q->e[i].pid_time > 0) {
        allFinished = 0;
        break;
      } 
    }
    
    if (allFinished == 1) break;
  }
  
  free(startTimes);
}

void runPQ(struct queue *q0, struct queue *q1, struct queue *q2)
{
  runSingleQueue(q0, 0);
  runSingleQueue(q1, 1);
  runSingleQueue(q2, 2);
  
}

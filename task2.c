#include "coursework.h"

#define MAX_PROCESSES 5

void generateSJF(struct queue *q);
void runSJF(struct queue *q, long **responseTimesOut, long **turnaroundTimesOut);
double average(long *array, int size);

int main()
{
  struct queue q;
  init(&q, MAX_PROCESSES);
  printAll(&q);
  generateSJF(&q);

  // run the code and store the response and turn around time of each process in arrays
  long *responseTimes = NULL;
  long *turnaroundTimes = NULL;
  runSJF(&q, &responseTimes, &turnaroundTimes);

  // calculate and print the average response and turn around times
  double averageResponseTime   = average(responseTimes, q.count);
  double averageTurnaroundTime = average(turnaroundTimes, q.count);
  printf("Average response time: %.2f milliseconds\n", averageResponseTime);
  printf("Average turn around time: %.2f milliseconds\n", averageTurnaroundTime);
  free(responseTimes);
  free(turnaroundTimes);

  freeAll(&q);
  return 0;
}

void generateSJF(struct queue *q)
{
  printf("Generating process for SJF ... \n");

  // the array is empty right after its been created
  // adding the first process to it makes it trivially sorted
  struct element p1 = generateProcess();
  printf("SJF: add first\n");
  addFirst(q, &p1);
  printAll(q);

  // generate MAX_PROCESSES - 1 more processes and put them in the queue
  // such that the largest pid_time is at index 0 of the array
  for (int i = 1; i < MAX_PROCESSES; i++)
  {
    struct element newProcess = generateProcess();
    // find the index where to insert the process based on pid_time
    // with the largest at the start
    int sortedIndex = 0;
    for (int j = 0; j < q->count; j++)
    {
      if (q->e[j].pid_time > newProcess.pid_time)
      {
        sortedIndex++;
      }
    }

    // add the process to the queue at its sorted index
    printf("SJF: add new largest\n");
    addHere(q, &newProcess, sortedIndex);
    printAll(q);
  }
}

void runSJF(struct queue *q, long **responseTimesOut, long **turnaroundTimesOut)
{
  long *responseTimes   = calloc(q->count, sizeof(int));
  long *turnaroundTimes = calloc(q->count, sizeof(int));
  // Created, Started, Ended
  // R = Created - Started 
  // T = Created - Ended
  
  // Stores the index to the response and turnaround times array that the program is currently on
  int timeIndex = 0;
  for (int i = q->count - 1; i >= 0; i--)
  {
    // run and time the process
    // the job ran will always be the last one in the array
    // which will always have the lowest pid_time
    struct timeval started;
    struct timeval ended;
    gettimeofday(&started, NULL);
    runNonPreemptiveJob(q, i);
    gettimeofday(&ended, NULL);

    // calculate the response and turn around times
    long int turnaroundTime = getDifferenceInMilliSeconds(q->e[i].created_time, ended);
    long int responseTime = getDifferenceInMilliSeconds(q->e[i].created_time, started);
    printf("C: %ld S: %ld E: %ld R:%ld T:%ld\n", q->e[i].created_time.tv_sec, started.tv_sec, ended.tv_sec, responseTime, turnaroundTime);

    // save response and turn around times
    responseTimes[timeIndex] = responseTime;
    turnaroundTimes[timeIndex] = turnaroundTime;
    timeIndex++;
  }

  *responseTimesOut = responseTimes;
  *turnaroundTimesOut = turnaroundTimes;
}

double average(long *array, int size)
{
  long sum = 0;
  for (int i = 0; i < size; i++)
  {
    // add the value of the the element in the array currently pointed to by i to sum
    sum += *(array + i);
  }

  return (double)sum / size;
}

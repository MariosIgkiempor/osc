// #include "osc_queue.h"
#include "coursework.h"
#define MAX_PROCESSES 5

void fifo(struct queue *q);
void lifo(struct queue *q);

int main()
{
  struct queue q;
  init(&q, MAX_PROCESSES);
  printAll(&q);
  fifo(&q);
  lifo(&q);
  freeAll(&q);

  return 0;
}

void fifo(struct queue *q)
{
  printf("Performing FIFO ...\n");
  for (int i = 0; i < MAX_PROCESSES; i++)
  {
    struct element e = generateProcess();
    addFirst(q, &e);
  }

  printAll(q);

  for (int i = 0; i < MAX_PROCESSES; i++)
  {
    removeLast(q);
    printAll(q);
  }
}

void lifo(struct queue *q)
{
  printf("Performing LIFO ...\n");
  for (int i = 0; i < MAX_PROCESSES; i++)
  {
    struct element e = generateProcess();
    addLast(q, &e);
  }

  printAll(q);

  for (int i = 0; i < MAX_PROCESSES; i++)
  {
    removeLast(q);
    printAll(q);
  }
}

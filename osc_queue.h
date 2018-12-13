#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

struct element
{
	int pid;
	int pid_time;
	int pid_priority;

	struct timeval created_time;
};

struct queue
{
	struct element *e;
	int max;
	int count;
};

int addFirst(struct queue *my_arr, struct element *new_e);
int addHere(struct queue *my_arr, struct element *new_e, int index);
int addLast(struct queue *my_arr, struct element *new_e);

void freeAll(struct queue *my_arr);
int getCount(struct queue *my_arr);
int init(struct queue *my_arr, int arr_size);
void printAll(struct queue *my_arr);

void removeLast(struct queue *my_arr);

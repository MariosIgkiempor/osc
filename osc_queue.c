#include "osc_queue.h"

int init(struct queue *my_arr, int arr_size)
{
  // Can't initialise a queue of size arr_size < 1 (no memory would be allocated)
  if (arr_size < 1)
  {
    printf("arr_size should be at least 1 (recieved %d)\n", arr_size);
    return 1;
  }

  // Allocate the array enough memory to store arr_size elements
  printf("Initialising ...\n");
  my_arr->e = malloc(arr_size * sizeof(struct element));

  // If malloc failed, print and return with error
  if (my_arr->e == NULL)
  {
    printf("Error allocating memory to the queue\n");
    return 1;
  }
  printf("Init: successfully malloc element with size of %d ...\n", arr_size);

  // Zero all the elements' fields
  for (int i = 0; i < arr_size; i++)
  {
    my_arr->e[i].pid = 0;
    my_arr->e[i].pid_priority = 0;
    my_arr->e[i].pid_time = 0;
    // Zero all the values inside the struct as well
    my_arr->e[i].created_time = (struct timeval){0};
  }

  // set count to 0 (there are no elements in the queue initially)
  // set the max to arr_size (can't have more elements than memory has been allocated for)
  my_arr->count = 0;
  my_arr->max = arr_size;
  return 0;
}

void freeAll(struct queue *my_arr)
{
  // array of elements is the only thing that has been malloc'ed (and so it's the only thing that should be freed)
  free(my_arr->e);
}

int getCount(struct queue *my_arr)
{
  // count will always contain the number of elements currently in the array
  // It will be incremented and decremented as things are added to/removed from the array
  return my_arr->count;
}

int addFirst(struct queue *my_arr, struct element *new_e)
{
  if (my_arr == NULL)
  {
    printf("Error adding to the start of the array: queue is NULL\n");
    return 1;
  }

  // error if the array is full
  if (my_arr->count >= my_arr->max)
  {
    printf("Error adding to the start of the array: queue is full\n");
    return 1;
  }

  // Shift all elements in the array one index to the right
  for (int i = my_arr->max - 2; i >= 0; i--)
  {
    my_arr->e[i + 1] = my_arr->e[i];
  }
  
  // Add the new element into the first index of the array
  my_arr->e[0] = *new_e;
 
  // increment the counter 
  my_arr->count++;

  printf("Add: pid %d into the queue with time %d\n", new_e->pid, new_e->pid_time);
  return 0;
}

int addLast(struct queue *my_arr, struct element *new_e)
{
  if (my_arr == NULL)
  {
    printf("Error adding to the end of the array: queue is NULL\n");
    return 1;
  }

  if (my_arr->count >= my_arr->max)
  {
    printf("Error adding to the end of the array: queue is full\n");
    return 1;
  }

  // Add the element to the last slot in the array and increment the counter
  my_arr->e[my_arr->count++] = *new_e;
  printf("Add: pid %d into the queue with time %d\n", new_e->pid, new_e->pid_time);
  return 0;
}

int addHere(struct queue *my_arr, struct element *new_e, int index)
{
  if (my_arr == NULL)
  {
    printf("Error adding to the middle of the array: queue is NULL\n");
    return 1;
  }

  if (my_arr->count >= my_arr->max)
  {
    printf("Error adding to the middle of the array: queue is full\n");
    return 1;
  }

  // If the given index is not a valid index in the array
  if (index >= my_arr->max || index < 0)
  {
    printf("Error adding to the middle of the array: invalid index\n");
    return 1;
  }

  // Shift all elements to the right of the index one place up to make room for the new element
  for (int i = my_arr->max - 2; i >= index; i--)
  {
    my_arr->e[i + 1] = my_arr->e[i];
  }
  my_arr->e[index] = *new_e;
  my_arr->count++;
  // printf("Add: pid %d into the queue with time %d into index %d\n", new_e->pid, new_e->pid_time, index);

  return 0;
}

void removeLast(struct queue *my_arr)
{
  if (my_arr == NULL)
  {
    printf("Error removing from the end of the array: queue is NULL\n");
  }
  int lastIndex = my_arr->count - 1;
  printf("Remove: %d from the queue index %d\n", my_arr->e[lastIndex].pid, lastIndex);
  // Removing an element means making all of its values zero (similar to re-initialising it)
  my_arr->e[lastIndex].pid = 0;
  my_arr->e[lastIndex].pid_priority = 0;
  my_arr->e[lastIndex].pid_time = 0;
  my_arr->e[lastIndex].created_time = (struct timeval){0};
  my_arr->count--;
}

void printAll(struct queue *my_arr)
{
  printf("There are %d elements in total\n", my_arr->count);
  for (int i = 0; i < my_arr->max; i++)
  {
    printf("#[%d]: %d running-time %d created-time %ld sec %d usec %d priority\n", i, my_arr->e[i].pid, my_arr->e[i].pid_time, my_arr->e[i].created_time.tv_sec, my_arr->e[i].created_time.tv_usec, my_arr->e[i].pid_priority);
  }
}

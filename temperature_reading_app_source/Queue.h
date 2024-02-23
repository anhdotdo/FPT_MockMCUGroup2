#ifndef _QUEUE_H_           // circular queue indeed
#define _QUEUE_H_

#include<stdint.h>
#include<stdio.h>

#define MAX_SIZE 10          // out of array length = 10

typedef enum{
    Queue_False,
    Queue_True,
}Queue_Bool_Type;

typedef struct{
    uint32_t QueueData[MAX_SIZE];
    int8_t Front;
    int8_t Rear;
}Queue_Type;

typedef enum{
    Q_OK,
    Q_IS_FULL,
    Q_IS_EMPTY,
}Queue_Status_Type;

Queue_Type Queue_GetVarQueue();
void Queue_Init();                                      // function to init the program
Queue_Status_Type Queue_Push(uint32_t data);            // function to push an element to tail
void Queue_ForcePush(uint32_t data);                    // function to push an element even full already
Queue_Status_Type Queue_Pop();                          // function to pop an element from head out of queue
Queue_Bool_Type Queue_isEmpty();                        // function to check status is empty?
Queue_Bool_Type Queue_isFull();                         // funciton to check status is full?

void IO_Display();                                      // function to display queue

/*
extend other functions
+ change MAX_SIZE
*/


#endif

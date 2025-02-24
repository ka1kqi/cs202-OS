#include "TaskQueue.h"


TaskQueue::
TaskQueue()
{
    // TODO: Your code here.
    //init queue mutex
    smutex_init(&tqm);
    scond_init(&tqc);
}

TaskQueue::
~TaskQueue()
{
    // TODO: Your code here.
    //destroy mutex
    smutex_unlock(&tqm);
    scond_destroy(&tqc);
    smutex_destroy(&tqm);
}

/*
 * ------------------------------------------------------------------
 * size --
 *
 *      Return the current size of the queue.
 *
 * Results:
 *      The size of the queue.
 *
 * ------------------------------------------------------------------
 */
int TaskQueue::
size()
{
    // TODO: Your code here.
    //same as empty
    return qsize; // Keep compiler happy until routine done.
}

/*
 * ------------------------------------------------------------------
 * empty --
 *
 *      Return whether or not the queue is empty.
 *
 * Results:
 *      true if the queue is empty and false otherwise.
 *
 * ------------------------------------------------------------------
 */
bool TaskQueue::
empty()
{
    // TODO: Your code here.
    //this assumes we already have a mutex so no need to lock and unlock
    return qsize==0; // Keep compiler happy until routine done.
}

/*
 * ------------------------------------------------------------------
 * enqueue --
 *
 *      Insert the task at the back of the queue.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void TaskQueue::
enqueue(Task task)
{
    // TODO: Your code here.
    smutex_lock(&tqm);
    tq.push(task);
    qsize++;
    //signal to deque that an item has been added
    scond_broadcast(&tqc,&tqm);
    smutex_unlock(&tqm);
}

/*
 * ------------------------------------------------------------------
 * dequeue --
 *
 *      Remove the Task at the front of the queue and return it.
 *      If the queue is empty, block until a Task is inserted.
 *
 * Results:
 *      The Task at the front of the queue.
 *
 * ------------------------------------------------------------------
 */
Task TaskQueue::
dequeue()
{
    // TODO: Your code here.
    smutex_lock(&tqm);

    while(empty())
        //wait for enqueue
        scond_wait(&tqc,&tqm);
    Task t=tq.front();
    tq.pop();
    qsize--;
    smutex_unlock(&tqm);
    return t; // Keep compiler happy until routine done.
}


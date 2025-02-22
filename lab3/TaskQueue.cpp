
#include "TaskQueue.h"
#include <queue>

std::queue<Task> tq;
smutex_t tqm;
scond_t tqc;

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
    //unlock and destroy mutex
    smutex_unlock(&tqm);
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
    smutex_lock(&tqm);
    int size=tq.size();
    smutex_unlock(&tqm);

    return size; // Keep compiler happy until routine done.
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
    smutex_lock(&tqm);
    bool empty=tq.empty();
    smutex_unlock(&tqm);
    return empty; // Keep compiler happy until routine done.
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
    //singal to deque
    scond_signal(&tqc,&tqm);
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

    while(size()==0)
        //wait for enqueue
        scond_wait(&tqc,&tqm);
    

    Task t=tq.front();
    tq.pop();
    smutex_unlock(&tqm);
    return t; // Keep compiler happy until routine done.
}


#pragma once

#include <queue>
#include <iostream>
#include "sthread.h"

typedef void (*handler_t) (void *); 

struct Task {
    handler_t handler;
    void* arg;
};

/*
 * ------------------------------------------------------------------
 * TaskQueue --
 * 
 *      A thread-safe task queue. This queue should be implemented
 *      as a monitor.
 *
 * ------------------------------------------------------------------
 */
class TaskQueue {
    private:
    // TODO: More needed here.
    std::queue<Task> tq;
    int qsize=0;
    smutex_t tqm;
    scond_t tqc;

    public:
    TaskQueue();
    ~TaskQueue();
    
    // no default copy constructor and assignment operators. this will prevent some
    // painful bugs by converting them into compiler errors.
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue &) = delete;

    void enqueue(Task task);
    Task dequeue();

    private:
    int size();
    bool empty();
};


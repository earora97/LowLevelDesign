# Task Scheduler in C++

## Overview
This project implements a multi-threaded task scheduler in C++ that can schedule both one-time and recurring tasks using a configurable number of worker threads.

Reference: https://akhiilgupta.medium.com/design-a-multi-threaded-task-scheduler-lld-multi-threaded-construct-eb090c5a8727

Problem Statement: Design a multi-threaded task scheduler with n threads which has the capability to schedule recurring tasks or one-time tasks.

Use Cases:
Job scheduled at a particular time
A recurring job with a particular interval of recurrence.
The user should be able to configure the number of worker threads
When there are an insufficient number of idle threads to perform the execution of all the tasks, we are going to execute the tasks with the latest execution time.

---

## Features
- **One-time and Recurring Tasks**: Supports scheduling of one-time tasks at specific timestamps and recurring tasks at fixed intervals.
- **Thread Management**: Allows the user to configure the number of worker threads.
- **Task Queue**: Uses a priority queue (min-heap) to ensure tasks are executed based on their scheduled time.
- **Synchronization**: Utilizes `std::mutex` and `std::condition_variable` for safe multi-threaded operations.
- **Graceful Shutdown**: The scheduler can be stopped manually, ensuring that all threads terminate safely.

---

## Code Structure
### Classes:
- **Task**: Abstract base class with common properties for all tasks.
- **OneTimeTask** and **RecurringTask**: Concrete implementations of `Task`.
- **TaskManager**: Manages the task queue, worker threads, and synchronization.

### Main Components:
- `TaskManager::worker()`: Worker thread function that picks and executes tasks.
- `TaskManager::addTask()`: Adds new tasks to the queue.
- `TaskManager::stopScheduler()`: Safely stops all threads.

---

## Build and Run
```bash
g++ -std=c++11 task_scheduler.cpp -o scheduler -lpthread
./scheduler
```

---

## Example Output:
```
Paying the bills!
Cleaning the house!
Cleaning the house!
...
```

---

## Future Enhancements
- Implement dynamic task addition/removal at runtime.
- Introduce logging and error handling.
- Improve time synchronization precision.

---

This implementation demonstrates key concepts of concurrency, synchronization, and task scheduling in modern C++.

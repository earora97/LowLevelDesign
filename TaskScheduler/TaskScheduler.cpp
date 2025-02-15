#include<iostream>
#include<functional>
#include<thread>
#include<cstdio>
#include<unistd.h>
#include<mutex>
#include<condition_variable>

using namespace std;

enum TaskType { OneTime, Recurring };

class Task{
protected:
    TaskType taskType;
    long executionTime;

public:
    Task(TaskType type, long scheduledTime = 0) : taskType(type) {
        executionTime = time(nullptr) + scheduledTime;
    }
    virtual void action() = 0; // pure virtual method
    virtual long getNextScheduledTime() { return -1; }
    // If your base class has at least one virtual function (like action())
    // always declare a virtual destructor.
    virtual ~Task() = default;

    TaskType getTaskType() const { return taskType; }
    long getExecutionTime() const { return executionTime; }
    void setExecutionTime(long newTime) { executionTime = newTime; }
};

class OneTimeTask : public Task {
    function<void()> taskAction;
public:
    OneTimeTask(function<void()> action, long scheduledTime = 0) 
        : Task(TaskType::OneTime, scheduledTime), taskAction(action) {}
    void action() override { taskAction(); }
};

class RecurringTask : public Task {
private:
    long frequency; // in seconds
    function<void()> taskAction;
public:
    RecurringTask(function<void()> action, int freq, long scheduledTime = 0) : 
    Task(TaskType::Recurring, scheduledTime), taskAction(action), frequency(freq) {}
    void action() override { taskAction(); }

    long getNextScheduledTime() override {
        return executionTime + frequency;
    }
};

struct CleanHouseTask {
    void operator()() { cout << "Cleaning the house!" << endl; }
};

struct PayBillsTask{
    void operator()() { cout << "Paying the bills!" << endl; }
};

struct CleanOfficeTask{
    void operator()() { cout << "Cleaning the office!" << endl; }
};

struct CompareTask {
    bool operator()(const pair<long, Task*>& a, const pair<long, Task*>& b) {
        return a.first > b.first;
    }
};

using PII = pair<long, Task*>;

class TaskManager{
private:
    // Priority queue is by default a max heap
    // custom comparator to make it a min heap
    priority_queue<PII, vector<PII>, CompareTask> tasks;
    int numThreads;
    vector<thread> threads;
    mutable mutex mtx;
    condition_variable cv;
    bool stop = false;

    void worker() {
        while (true) {
            Task* currentTask = nullptr;
            {
                if (tasks.empty() || stop) return;
                unique_lock<mutex> lock(mtx);
                cv.wait(lock, [this] { return !tasks.empty(); });

                auto nextWorkItem = tasks.top();
                if (nextWorkItem.first > time(nullptr)) {
                    cv.wait_until(lock, chrono::system_clock::from_time_t(nextWorkItem.first));
                    continue;
                }
                tasks.pop();
                currentTask = nextWorkItem.second;
            }

            currentTask->action();

            if (currentTask->getTaskType() == Recurring) {
                long nextScheduledTime = currentTask->getNextScheduledTime();
                currentTask->setExecutionTime(nextScheduledTime);
                AddTask(currentTask);
            } else {
                delete currentTask;
            }
        }
    }
public:
    TaskManager(int n): numThreads(n) {
        for (int i = 0; i < numThreads; ++i) {
            // threads.emplace_back constructs a thread directly in the vector.
            threads.emplace_back([this]() { this->worker(); });
        }
    }

    void stopScheduler() {
        {
            lock_guard<mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
    }

    ~TaskManager() {
        cout << "Done for the day!" << endl;
        stopScheduler();
        for (auto& t : threads)
            t.join();
    }

    void AddTask(Task* task) {
        {
            lock_guard<mutex> lock(mtx);
            tasks.push({task->getExecutionTime(), task});
        }
        cv.notify_one();
    }
};

int main() {
    // Onetime task to pay bills, scheduled at time 1
    OneTimeTask *task1 = new OneTimeTask(PayBillsTask(), 0);
    // Onetime task to clean house, scheduled at time 1
    OneTimeTask *task2 = new OneTimeTask(CleanOfficeTask(), 1);
    // Recurring task to clean house with recurring interval of 2, scheduled at time 5 
    RecurringTask *task3 = new RecurringTask(CleanHouseTask(), 2, 5);

    // Task Manager with maximum 2 concurrent threads
    TaskManager manager(2);
   
    manager.AddTask(task1);
    manager.AddTask(task2);
    manager.AddTask(task3);

    sleep(10);
    manager.stopScheduler();  // Manual stop
    return 0;
}
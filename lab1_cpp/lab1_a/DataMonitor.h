#ifndef DATAMONITOR_H
#define DATAMONITOR_H

#include <mutex>
#include <condition_variable>
#include "Car.h"

class DataMonitor
{
private:
    int capacity;
    struct car *cars;
    int count;
    int last_index;
    std::mutex mut;
    std::condition_variable cv;
public:
    DataMonitor(int cap);
    void insert(car c);
    car remove();
    bool is_empty();
    int size();
};
#endif
#include "DataMonitor.h"
#include <iostream>

DataMonitor::DataMonitor(int cap)
{
    capacity = cap;
    cars = new car[capacity];
    count = 0;
    front = -1;
    rear = -1;
}

void DataMonitor::insert(car c)
{
    std::unique_lock<std::mutex> guard(mut);
	cv.wait(guard, [&]{ return count < capacity;});

  
    cars[count ++] = c;
    cv.notify_all();
}

car DataMonitor::remove()
{
    std::unique_lock<std::mutex> guard(mut);
    cv.wait(guard, [&]{ return count > 0; });

    count --;
    cv.notify_all();
    return cars[count];
}

bool DataMonitor::is_empty()
{
    std::unique_lock<std::mutex> guard(mut);
    return count == 0;
}

int DataMonitor::size(){
    return count;
}
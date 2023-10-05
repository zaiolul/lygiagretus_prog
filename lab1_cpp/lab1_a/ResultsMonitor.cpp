
#include "ResultsMonitor.h"
#include <iostream>

ResultsMonitor::ResultsMonitor(int cap)
{
    cars = new car[cap];
    count = 0;
}

void ResultsMonitor::insert_sorted(car c)
{
    mut.lock();
    count ++;
    for(int i = 0; i < count ; i ++){
        if(cars[i].year <= c.year){
            for(int j = count - 1 ; j > i ; j --){
                cars[j] = cars[j - 1];
            }
            cars[i] = c;
            break;
        }
    }
    mut.unlock();
}

//tiesiog istraukt masyvus printui
void ResultsMonitor::set_data(car **c)
{
   *c = cars;
}
//pasitikrinimui
int ResultsMonitor::size()
{
    return count;
}
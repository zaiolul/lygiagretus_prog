
#include "ResultsMonitor.h"
#include <iostream>
ResultsMonitor::ResultsMonitor(int cap)
{
    cars = new car[cap];
    calculated_values = new int[cap];
    count = 0;
}

void ResultsMonitor::insert_sorted(car c, int calculated_value)
{
    std::unique_lock<std::mutex> guard(mut);
    count ++;
    for(int i = 0; i < count ; i ++){
        if(cars[i].year <= c.year){
            for(int j = count - 1 ; j > i ; j --){
                cars[j] = cars[j - 1];
                calculated_values[j] = calculated_values[j - 1];
            }
            cars[i] = c;
            calculated_values[i] = calculated_value;
            break;
        }
    }
}

//tiesiog istraukt masyvus printui
void ResultsMonitor::set_data(car **c, int **d)
{
   *c = cars;
   *d = calculated_values;
}
int ResultsMonitor::size()
{
    return count;
}
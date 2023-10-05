#include "Car.h"
#include <condition_variable>
#include <mutex>

class ResultsMonitor
{
private:
    struct car *cars;
    int *calculated_values;
    int count;
    std::mutex mut;
public:
    ResultsMonitor(int cap);
    void insert_sorted(car c);
    void set_data(car **c);
    int size();
};
#include "car.h"
#include <condition_variable>
#include <mutex>

class ResultsMonitor
{
private:
    struct car *cars;
    int *calculated_values;
    int count;
    std::mutex mut;
    std::condition_variable cv;
public:
    ResultsMonitor(int cap);
    void insert_sorted(car c, int calculated_value);
    void set_data(car **c, int **d);
    int size();
};
#ifndef CAR_H
#define CAR_H
#include <string>

#define THREAD_COUNT 8

struct car{
    std::string model;
    int year;
    double engine_volume;
    int calculated_value;
};

#endif 
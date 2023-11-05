#ifndef CAR_H
#define CAR_H
#include <string>
#define THREAD_COUNT 4

#define MAX_LEN 255
struct car{
    char model[MAX_LEN];
    int year;
    double engine_volume;
    int calculated_value;
};

#endif 
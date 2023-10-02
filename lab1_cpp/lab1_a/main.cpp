#include "DataMonitor.h"
#include "ResultsMonitor.h"
#include "../json.hpp"

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <math.h>

using json = nlohmann::json;

#define THREAD_COUNT 4

#define RESULTS_COUNT 255
#define DATA_COUNT 5

#define OUTPUT_FILE "../IFK_1_SabaliauskasE_rez.txt"

bool finished_reading = false;


int calc(car c)
{   
    int result = 1;
    int number = c.model.length() + c.year + int(c.engine_volume);
    for(int i = 1; i <= number ; i ++){
        for(int j = 0; j < number * 100 ; j ++){
            result += i;
        }
        result = i ;
    }

    return result;
}

void print_results(std::ofstream &out, ResultsMonitor *results)
{
    car *ptr_cars;
    int *ptr_values;
    results->set_data(&ptr_cars,  &ptr_values);
    out << std::left << std::setw(25) << "Modelis" << "|" << std::setw(10) << "Metai" << "|" << 
        std::setw(16) << "Darbinis tūris" << "|" << std::setw(20) << "Skaičiuota Reikšmė" << "|" << std::endl;
    out << std::setfill('-');
    out << std::setw(25) << "" << "|" << std::setw(10) << "" << "|" << std::setw(15) << ""<< std::setw(20)<< "|" << std::endl;
    out << std::setfill(' ');

    for (int i = 0; i < results->size(); i ++) {
        out << std::left << std::setw(25) << ptr_cars[i].model << "|" << std::setw(10) << std::right << ptr_cars[i].year << "|" << 
            std::setw(15) << std::fixed << std::setprecision(1) << ptr_cars[i].engine_volume << "|" <<  std::setw(18) <<  ptr_values[i] << "|" <<std::endl;
    }
}

void thread_func(DataMonitor *data_m, ResultsMonitor *results_m, int num)
{
    while(!finished_reading || data_m->size() > 0){
     
        car c = data_m->remove();
        
        if(c.year == 0) 
            continue;

        int calculated_value = calc(c);

        if(calculated_value % 2 != 0) // salyga, tik lyginiai
            continue;

        results_m->insert_sorted(c, calculated_value);
    
    }
}

int main(int argc, char **argv)
{
    std::ifstream file_in(argv[1]);
	json data = json::parse(file_in);

    std::vector<car> car_data;

    for (auto& it : data) {
        car c = car{.model = it["model"], .year = it["year"], .engine_volume = it["enginevolume"]};
		car_data.push_back(c);
	}

    DataMonitor data_m(DATA_COUNT);
    ResultsMonitor results_m(RESULTS_COUNT);

    std::vector<std::thread> threads;
    for(int i = 0; i < THREAD_COUNT; i ++){
        threads.push_back(std::thread(thread_func, &data_m, &results_m, i));
    }
    for(car c : car_data){
        data_m.insert((c));
    }
  
    finished_reading = true;

    for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
    
    std::ofstream out(OUTPUT_FILE);
    print_results(out, &results_m);
    out.close();

    return 0;
}
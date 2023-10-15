#include "DataMonitor.h"
#include "ResultsMonitor.h"
#include "../json.hpp"

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <math.h>

using json = nlohmann::json;

#define RESULTS_COUNT 255
#define DATA_COUNT 5

#define OUTPUT_FILE "../../IFK_1_SabaliauskasE_rez.txt"
/* RUN TEST */
// #define RUN_TEST
/*----------*/
#ifdef RUN_TEST
#define ITERATIONS = 100
#endif

bool finished = false;
std::mutex mut;

bool is_finished(bool set){
    std::unique_lock<std::mutex> guard(mut);
    if(set){
        finished = true;
    }
    return finished == true;
}

int calc(car c)
{   
    int result = 1;
    int number = c.model.length() + c.year + int(c.engine_volume);
    for(int i = 1; i <= number ; i ++){
        for(int j = 0; j < number  *10; j ++){
            result += i;
        }
        result = i ;
    }

    return result;
    // return 2;
}

void print_results(std::ofstream &out, ResultsMonitor *results)
{
    car *ptr_cars;
    results->set_data(&ptr_cars);

    out << std::left << std::setw(25) << "Modelis" << "|" << std::setw(10) << "Metai" << "|" << 
        std::setw(16) << "Darbinis tūris" << "|" << std::setw(20) << "Skaičiuota Reikšmė" << "|" << std::endl;
    out << std::setfill('-');
    out << std::setw(25) << "" << "|" << std::setw(10) << "" << "|" << std::setw(15) << ""<< std::setw(20)<< "|" << std::endl;
    out << std::setfill(' ');

    for (int i = 0; i < results->size(); i ++) {
        out << std::left << std::setw(25) << ptr_cars[i].model << "|" << std::setw(10) << std::right << ptr_cars[i].year << "|" << 
            std::setw(15) << std::fixed << std::setprecision(1) << ptr_cars[i].engine_volume << "|" <<  std::setw(18) <<  ptr_cars[i].calculated_value << "|" <<std::endl;
    }
}

void thread_func(DataMonitor *data_m, ResultsMonitor *results_m)
{
    int count = 0;
    while(true){
        car c = data_m->remove();
        if(c.year == 0){
            std::cout << "breaking" << std::endl;
            break;
        }
            
        c.calculated_value = calc(c);
       // std::cout << c.year << std::endl;
       
        if(c.calculated_value % 2 != 0) // salyga, tik lyginiai
            continue;
        results_m->insert_sorted(c);
        count ++;
    }
    // std::cout << "-" << count << "-"<< std::endl;
   
}

int main(int argc, char **argv)
{
    std::ifstream file_in(argv[1]);
	json data = json::parse(file_in);

#ifdef RUN_TEST
for(int i = 0; i < 200; i ++){
#endif
    std::vector<car> car_data;

    DataMonitor data_m(DATA_COUNT);
    ResultsMonitor results_m(RESULTS_COUNT);
   
    for (auto& it : data) {
        car c = car{.model = it["model"], .year = it["year"], .engine_volume = it["enginevolume"]};
		car_data.push_back(c);
	}
  
    std::vector<std::thread> threads;
    for(int i = 0; i < THREAD_COUNT; i ++){
        threads.push_back(std::thread(thread_func, &data_m, &results_m));
    }
    for(car c : car_data){
        std::cout << c.model << " " << c.year << std::endl;
        data_m.insert((c));
    }
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        data_m.insert_first(car{});
    }
   
    for (int i = 0; i < THREAD_COUNT; i++) {
	
    	threads[i].join();
	}
    std::cout << "data size: " << data_m.size() << std::endl;
   
    std::ofstream out(OUTPUT_FILE);
    print_results(out, &results_m);
    out.close();

#ifdef RUN_TEST
    }
#endif

    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <omp.h>

#include "../json.hpp"
#include "Car.h"

using json = nlohmann::json;

#define THREAD_COUNT 4

#define OUTPUT_FILE "../../IFK_1_SabaliauskasE_rez.txt"
int calc(car c)
{   
    int result = 1;
    int number = c.model.length() + c.year + int(c.engine_volume);
    for(int i = 1; i <= number ; i ++){
        for(int j = 0; j < number  ; j ++){
            result += i;
        }
        result = i;
    }

    return result;
    // return 2;
}

// void print_results(std::ofstream &out,, std::vector<car> results)
// {

 

//     out << std::left << std::setw(25) << "Modelis" << "|" << std::setw(10) << "Metai" << "|" << 
//         std::setw(16) << "Darbinis tūris" << "|" << std::setw(20) << "Skaičiuota Reikšmė" << "|" << std::endl;
//     out << std::setfill('-');
//     out << std::setw(25) << "" << "|" << std::setw(10) << "" << "|" << std::setw(15) << ""<< std::setw(20)<< "|" << std::endl;
//     out << std::setfill(' ');

//     for (int i = 0; i < results->size(); i ++) {
//         out << std::left << std::setw(25) << ptr_cars[i].model << "|" << std::setw(10) << std::right << ptr_cars[i].year << "|" << 
//             std::setw(15) << std::fixed << std::setprecision(1) << ptr_cars[i].engine_volume << "|" <<  std::setw(18) <<  ptr_cars[i].calculated_value << "|" <<std::endl;
//     }
// }

int main(int argc, char **argv)
{
    std::ifstream file_in(argv[1]);
	json data = json::parse(file_in);
    
    std::vector<car> car_data;
    std::tuple<int,int> range[THREAD_COUNT];

    for (auto& it : data) {
        car c = car{.model = it["model"], .year = it["year"], .engine_volume = it["enginevolume"]};
		car_data.push_back(c);
	}
    
    int part = car_data.size() / THREAD_COUNT ;
    int rem = car_data.size() % THREAD_COUNT;

    int end = 0;
    int start = 0;

    for(int i = 0; i < THREAD_COUNT; i ++){
        int n = i < rem ? part + 1 : part;
        start = end;
        end += n;
        range[i] = std::make_tuple(start, end);
        // std::cout << std::get<0>(range[i]) <<  " to " << std::get<1>(range[i]) << std::endl;
    }



 
    return 0;
}
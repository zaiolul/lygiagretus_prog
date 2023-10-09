#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <omp.h>

#include "../json.hpp"
#include "Car.h"

using json = nlohmann::json;

#define OUTPUT_FILE "../../IFK_1_SabaliauskasE_rez.txt"
int calc(car c)
{   
    int result = 1;
    int number = c.model.length() + c.year + int(c.engine_volume);
    for(int i = 1; i <= number ; i ++){
        for(int j = 0; j < number *100 ; j ++){
            result += i;
        }
        result = i;
    }

    return result;
}

void insert_sorted(std::vector<car> *results, car c, int val)
{
    std::vector<car> copy = *results;
    c.calculated_value = val;
    copy.push_back(car{});
    for(int i = 0; i < copy.size(); i ++){
        if(copy[i].year <= c.year){
            for(int j = copy.size() - 1 ; j > i ; j --){
                copy[j] = copy[j - 1];
            }
            copy[i] = c;
            break;
        }
    }
    *results = copy;
    
}
void print_results(std::ofstream &out, std::vector<car> *results, int sum_int, float sum_float)
{

    std::vector<car> r = *results;

    out << std::left << std::setw(25) << "Modelis" << "|" << std::setw(10) << "Metai" << "|" << 
        std::setw(16) << "Darbinis tūris" << "|" << std::setw(20) << "Skaičiuota Reikšmė" << "|" << std::endl;
    out << std::setfill('-');
    out << std::setw(25) << "" << "|" << std::setw(10) << "" << "|" << std::setw(15) << ""<< std::setw(20)<< "|" << std::endl;
    out << std::setfill(' ');

    for (int i = 0; i < r.size(); i ++) {
        out << std::left << std::setw(25) << r[i].model << "|" << std::setw(10) << std::right << r[i].year << "|" << 
            std::setw(15) << std::fixed << std::setprecision(1) << r[i].engine_volume << "|" <<  std::setw(18) <<  r[i].calculated_value << "|" <<std::endl;
    }
    out << std::setfill(' ');
    out << "int laukų suma: " << sum_int << "; float laukų suma: " << sum_float << std::endl;
}

int main(int argc, char **argv)
{
    std::ifstream file_in(argv[1]);
	json data = json::parse(file_in);
    
    std::vector<car> car_data;
    int range[THREAD_COUNT][2];

    for (auto& it : data) {
        car c = car{.model = it["model"], .year = it["year"], .engine_volume = it["enginevolume"]};
		car_data.push_back(c);
	}
    
    // DUOMENU INTERVALAI THREADAMS
    int part = car_data.size() / THREAD_COUNT;
    int rem = car_data.size() % THREAD_COUNT;

    int end = 0;
    int start = 0;

    for(int i = 0; i < THREAD_COUNT; i ++){
        int n = i < rem ? part + 1 : part;
        start = end;
        end += n;
        range[i][0] = start;
        range[i][1] = end;
        std::cout << i << " threadas: [" << start << ";" << end << "), viso kiekis: " << end - start - 1 << std::endl;
    }
    //--------------------------------------
    std::vector<car> results;
    results.reserve(car_data.size());
    
    float sum_float = 0;
    int sum_int = 0;

#pragma omp parallel num_threads(THREAD_COUNT) reduction(+:sum_int, sum_float) shared(range, results, car_data)
    {
        int thread_num = omp_get_thread_num();
        int start = range[thread_num][0];
        int end = range[thread_num][1];
        for(int i = start; i < end; i ++){
            int val = calc(car_data[i]);
            if(val % 2 != 0)
                continue;
            
            sum_int += car_data[i].year;
            sum_float += car_data[i].engine_volume;
#pragma omp critical
            {
                insert_sorted(&results, car_data[i], val);
            }
        }
    }
    
    // for(auto c : results){
    //     std::cout << c.model << " " << c.year << std::endl;
    // }
    // std::cout << results.size() << " " << sum_int << " " << sum_float<<  std::endl;
    

    std::ofstream out(OUTPUT_FILE);
    print_results(out, &results, sum_int, sum_float);

 
    return 0;
}
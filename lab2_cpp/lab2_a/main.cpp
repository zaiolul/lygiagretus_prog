// #include "DataMonitor.h"
// #include "ResultsMonitor.h"
#include "../json.hpp"
#include "Car.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <mpi.h>

using json = nlohmann::json;

#define RESULTS_COUNT 255
#define DATA_COUNT    5

#define OUTPUT_FILE "../../IFK_1_SabaliauskasE_rez.txt"
/* RUN TEST */
// #define RUN_TEST
/*----------*/
#ifdef RUN_TEST
#define ITERATIONS = 100
#endif

using namespace std;
using namespace MPI;

enum{
    MAIN = 0,
    DATA = 1,
    RESULTS = 2
};
void insert_sorted(car *cars, int *count, car c)
{
	(*count)++;
	for (int i = 0; i < *count; i++) {
		if (cars[i].year <= c.year) {
			for (int j = (*count) - 1; j > i; j--) {
				cars[j] = cars[j - 1];
			}
			cars[i] = c;
			break;
		}
	}
}

// int calc(car c)
// {
// 	int result = 1;
// 	int number = c.model.length() + c.year + int(c.engine_volume);
// 	for (int i = 1; i <= number; i++) {
// 		for (int j = 0; j < number * 10; j++) {
// 			result += i;
// 		}
// 		result = i;
// 	}

// 	return result;
// 	// return 2;
// }

// void print_results(ofstream &out, ResultsMonitor *results)
// {
// 	car *ptr_cars;
// 	results->set_data(&ptr_cars);

// 	out << left << setw(25) << "Modelis"
// 	    << "|" << setw(10) << "Metai"
// 	    << "|" << setw(16) << "Darbinis tūris"
// 	    << "|" << setw(20) << "Skaičiuota Reikšmė"
// 	    << "|" << endl;
// 	out << setfill('-');
// 	out << setw(25) << ""
// 	    << "|" << setw(10) << ""
// 	    << "|" << setw(15) << "" << setw(20) << "|" << endl;
// 	out << setfill(' ');

// 	for (int i = 0; i < results->size(); i++) {
// 		out << left << setw(25) << ptr_cars[i].model << "|" << setw(10) << right << ptr_cars[i].year
// 		    << "|" << setw(15) << fixed << setprecision(1) << ptr_cars[i].engine_volume << "|"
// 		    << setw(18) << ptr_cars[i].calculated_value << "|" << endl;
// 	}
// }

// void thread_func(DataMonitor *data_m, ResultsMonitor *results_m)
// {
// 	int count = 0;
// 	while (true) {
// 		car c = data_m->remove();
// 		if (c.year == 0) {
// 			cout << "breaking" << endl;
// 			break;
// 		}

// 		c.calculated_value = calc(c);
// 		// cout << c.year << endl;

// 		if (c.calculated_value % 2 != 0) // salyga, tik lyginiai
// 			continue;
// 		results_m->insert_sorted(c);
// 		count++;
// 	}
// 	// cout << "-" << count << "-"<< endl;
// }

int main(int argc, char **argv)
{
	// #ifdef RUN_TEST
	// 	for (int i = 0; i < 200; i++) {
	// #endif
	Init();
    int types = 3; //int, float, string
    MPI_Datatype car_types[types] = {CHAR, INT, DOUBLE, INT};
	int car_blocks[types] = {MAX_LEN, 1, 1, 1};
	
	MPI_Datatype mpi_car_type; // struktura zinutem

	Aint offsets[types] = {offsetof(car, model), offsetof(car, year), offsetof(car, engine_volume), offsetof(car, calculated_value)};
	MPI_Type_create_struct(types, car_blocks, offsets, car_types, &mpi_car_type);
	MPI_Type_commit(&mpi_car_type);

	
	auto rank = COMM_WORLD.Get_rank();
	if (rank == MAIN) { //main
		ifstream file_in(argv[1]);
		json data = json::parse(file_in);
		vector<car> car_data;
		
		for (auto &it : data) {
			string str = it["model"];
			car c = car{
				     .year	    = it["year"],
				     .engine_volume = it["enginevolume"] };
			strncpy(c.model, str.c_str(), str.size());
			car_data.push_back(c);
            
	
    
		}
		int size = car_data.size();

		COMM_WORLD.Send(&size, 1, INT, DATA, 1);
		for (auto &it : car_data) {
			COMM_WORLD.Send(&it, 1, mpi_car_type, DATA, 1);
		}

		
	} else if (rank == DATA) { //data
		Status status;  // store message status here
        COMM_WORLD.Probe(ANY_SOURCE, 1, status);  // check incoming message status
		int size;  // store received message here
		
        COMM_WORLD.Recv(&size, 1, INT, MAIN, 1);  // receive the message
		cout << size << endl;
		for(int i = 0; i < size; i ++){
			struct car rec;

			COMM_WORLD.Recv(&rec, 1, mpi_car_type, MAIN, 1);  // receive the message
			cout << rec.model << rec.year << rec.engine_volume << endl;
		}
	} else if (rank == RESULTS) { //results

	} else { //workers

	}

	// vector<thread> threads;
	// for(int i = 0; i < THREAD_COUNT; i ++){
	//     threads.push_back(thread(thread_func, &data_m, &results_m));
	// }
	// for(car c : car_data){
	//     cout << c.model << " " << c.year << endl;
	//     data_m.insert((c));
	// }

	// for (int i = 0; i < THREAD_COUNT; i++) {
	//     data_m.insert_first(car{});
	// }

	// for (int i = 0; i < THREAD_COUNT; i++) {

	// 	threads[i].join();
	// }
	// cout << "data size: " << data_m.size() << endl;

	// ofstream out(OUTPUT_FILE);
	// print_results(out, &results_m);
	// out.close();

	// #ifdef RUN_TEST
	// 	}
	// #endif
	MPI_Type_free(&mpi_car_type);
	Finalize();
	return 0;
}
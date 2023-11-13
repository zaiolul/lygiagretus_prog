#include "../json.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>
#include <unistd.h>
#include <string>

using json = nlohmann::json;

#define RESULTS_COUNT 255
#define DATA_COUNT 10

#define OUTPUT_FILE "../IFK_1_SabaliauskasE_rez.txt"
/* RUN TEST */
// #define RUN_TEST
/*----------*/
#ifdef RUN_TEST
#define ITERATIONS = 100
#endif

using namespace std;
using namespace MPI;

class Car
{
private:
	string model;
	int year;
	double engine_volume;
	int calculated_value;

public:
	string get_model()
	{
		return model;
	}
	int get_year()
	{
		return year;
	}
	double get_engine_volume()
	{
		return engine_volume;
	}
	int get_calculated_value()
	{
		return calculated_value;
	}
	void set_calculated_value(int val)
	{
		calculated_value = val;
	}

	Car(string model, int year, double engine_volume, int calculated_value)
	{
		this->model = model;
		this->year = year;
		this->engine_volume = engine_volume;
		this->calculated_value = calculated_value;
	}

	Car()
	{
	}

	string serialize()
	{
		json j;

		j["model"] = model;
		j["year"] = year;
		j["engine_volume"] = engine_volume;
		j["calculated_value"] = calculated_value;

		// cout << j["model"] << endl;
		// return "AA";
		return to_string(j);
	}

	static Car deserialize(string json_str)
	{
		json data = json::parse(json_str);
		return Car(data["model"], data["year"], data["engine_volume"], data["calculated_value"]);
	}

	void print_car()
	{
		cout << model << " " << year << " " << engine_volume << " " << calculated_value << endl;
	}
};

void print_results(ofstream &out, vector<Car> results)
{
	out << left << setw(25) << "Modelis"
		<< "|" << setw(10) << "Metai"
		<< "|" << setw(16) << "Darbinis tūris"
		<< "|" << setw(20) << "Skaičiuota Reikšmė"
		<< "|" << endl;
	out << setfill('-');
	out << setw(25) << ""
		<< "|" << setw(10) << ""
		<< "|" << setw(15) << "" << setw(20) << "|" << endl;
	out << setfill(' ');

	for (int i = 0; i < results.size(); i++)
	{
		// results[i].print_car();
		out << left << setw(25) << results[i].get_model() << "|" << setw(10) << right
			<< results[i].get_year() << "|" << setw(15) << fixed << setprecision(1)
			<< results[i].get_engine_volume() << "|" << setw(18) << results[i].get_calculated_value()
			<< "|" << endl;
	}
}

void insert_sorted(Car cars[], int *count, Car c)
{
	(*count)++;
	for (int i = 0; i < *count; i++)
	{
		if (cars[i].get_year() <= c.get_year())
		{
			for (int j = (*count) - 1; j > i; j--)
			{
				cars[j] = cars[j - 1];
			}
			cars[i] = c;
			break;
		}
	}
}

int calc(Car c)
{
	int result = 1;
	int number = c.get_model().length() + c.get_year() + int(c.get_engine_volume());
	for (int i = 1; i <= number; i++)
	{
		for (int j = 0; j < number * 10; j++)
		{
			result += i;
		}
		result = i;
	}
	return result;
}

enum
{
	MAIN = 0,
	DATA = 1,
	RESULTS = 2,
};

enum
{
	CHECK = 1,
	END = 2,
	REMOVE = 3
};

int main(int argc, char **argv)
{
	Init();
	int should_end = 0;
	int rank = COMM_WORLD.Get_rank();

	if (COMM_WORLD.Get_size() < 5)
		exit(1);

	cout << "rank " << rank << endl;

	if (rank == MAIN)
	{ // main
		ifstream file_in(argv[1]);
		json data = json::parse(file_in);
		vector<Car> car_data;

		for (auto &it : data)
		{
			Car car(it["model"], it["year"], it["enginevolume"], 0);

			car_data.push_back(car);

			string car_str = car.serialize();
			const char *car_cstr = car_str.c_str();
			COMM_WORLD.Send(car_cstr, car_str.size(), CHAR, DATA, 0);
			usleep(1000);
		}
		COMM_WORLD.Send(NULL, 0, CHAR, DATA, END);

		Status status;

		vector<Car> car_results;
		int run = 1;
		while (run)
		{
			COMM_WORLD.Probe(RESULTS, ANY_TAG, status);
			if (status.Get_tag() == END)
			{
				COMM_WORLD.Recv(NULL, 0, CHAR, RESULTS, END);
				run = 0;
			}
			else
			{
				int size = status.Get_count(CHAR);
				char str[size];
				COMM_WORLD.Recv(str, size, CHAR, RESULTS, 0);
				car_results.push_back(Car::deserialize(string(str, str + size)));
			}
		}
		ofstream out(OUTPUT_FILE);
		print_results(out, car_results);
		out.close();
	}
	else if (rank == DATA)
	{
		const int CAP = DATA_COUNT;
		Car data[CAP];
		Status status;
		int count = 0;

		while (true)
		{
			if (count == 0 && should_end == 1)
				break;

			if (count < CAP)
			{
				COMM_WORLD.Probe(MAIN, ANY_TAG, status);
				if (status.Get_tag() == END)
				{
					COMM_WORLD.Recv(NULL, 0, CHAR, MAIN, END);
					should_end = 1;
				}

				if(should_end)
					continue;
				int size = status.Get_count(CHAR);
				char str[size];

				COMM_WORLD.Recv(str, size, CHAR, MAIN, ANY_TAG);

				Car car = Car::deserialize(string(str, str + size));
				data[count++] = car;
			}

			if (count > 0)
			{
				COMM_WORLD.Probe(ANY_SOURCE, CHECK, status);
		
				COMM_WORLD.Recv(NULL, 0, CHAR, status.Get_source(), CHECK);
			
				COMM_WORLD.Send(NULL, 0, CHAR, status.Get_source(), CHECK);

				string car_str = data[count - 1].serialize();
				count--;
				const char *car_cstr = car_str.c_str();
				COMM_WORLD.Send(car_cstr, car_str.size(), CHAR, status.Get_source(), 0);
			}
		}

		for (int i = RESULTS + 1; i < COMM_WORLD.Get_size(); i++)
		{
			COMM_WORLD.Send(NULL, 0, CHAR, i, END);
		}
	}
	else if (rank == RESULTS)
	{
		Car results[RESULTS_COUNT];
		Status status;
		int count = 0;
		int worker_count = COMM_WORLD.Get_size() - RESULTS - 1;

		while (worker_count > 0)
		{
			COMM_WORLD.Probe(ANY_SOURCE, ANY_TAG, status);
			if (status.Get_tag() == END)
			{

				COMM_WORLD.Recv(NULL, 0, CHAR, ANY_SOURCE, END);
				worker_count--;
			}
			else
			{
				int size = status.Get_count(CHAR);
				char str[size];
				COMM_WORLD.Recv(str, size, CHAR, ANY_SOURCE, ANY_TAG);
				insert_sorted(results, &count, Car::deserialize(string(str, str + size)));
			}
		}

		for (int i = 0; i < count; i++)
		{
			string car_str = results[i].serialize();
			const char *car_cstr = car_str.c_str();
			COMM_WORLD.Send(car_cstr, car_str.size(), CHAR, MAIN, 0);
		}
		COMM_WORLD.Send(NULL, 0, CHAR, MAIN, END);
	}
	else
	{
		Status status;
		while (true)
		{
			COMM_WORLD.Send(NULL, 0, CHAR, DATA, CHECK);

			COMM_WORLD.Probe(DATA, ANY_TAG, status);
			if (status.Get_tag() == CHECK)
			{
				COMM_WORLD.Recv(NULL, 0, CHAR, DATA, CHECK);
				continue;
			}
			else if (status.Get_tag() == END)
			{
				COMM_WORLD.Recv(NULL, 0, CHAR, DATA, END);
				break;
			}
			else
			{
				int size = status.Get_count(CHAR);
				char str[size];
				COMM_WORLD.Recv(str, size, CHAR, DATA, ANY_TAG);

				Car car = Car::deserialize(string(str, str + size));
				int val = calc(car);
				if (val % 2 == 0)
				{
					car.set_calculated_value(val);
					string car_str = car.serialize();
					const char *car_cstr = car_str.c_str();
					COMM_WORLD.Send(car_cstr, car_str.size(), CHAR, RESULTS, 0);
				}
			}
		}
		COMM_WORLD.Send(NULL, 0, CHAR, RESULTS, END);
	}

	Finalize();
	return 0;
}
using System;
using System.ComponentModel;

namespace lab1a{
    class ProgA{
        public static int Main(string[] args){
           
            DataMonitor dm = new DataMonitor();

            dm.AddCar(new Car("test1", 2000, 2.5));
            dm.AddCar(new Car("test2", 2010, 3.0));

            for(int i = 0; i < 10; i++){
                Car car = dm.RemoveCar();
                if(car == null){
                    Console.WriteLine("car null");
                    continue;
                }
                Console.WriteLine(string.Format("{0} {1} {2}", car.Model, car.Year, car.EngineVolume));
            }
      
            dm.AddCar(new Car("test3", 999, 11.1));
            Car car1 = dm.RemoveCar();
            Console.WriteLine(string.Format("{0} {1} {2}", car1.Model, car1.Year, car1.EngineVolume));
            return 0;
        }
    }
}



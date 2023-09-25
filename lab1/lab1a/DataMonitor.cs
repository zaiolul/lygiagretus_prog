using System;

namespace lab1a{
    public class DataMonitor{
        int capacity;
        Car[] cars;
        int lastIndex;
        public DataMonitor(int capacity = 32){
            this.capacity = capacity;
            cars = new Car[capacity];
            lastIndex = -1;
        }

        public bool AddCar(Car car){
            if(lastIndex == capacity){
                return false;
            }

            cars[++lastIndex] = car;

            return true;
        }

        public Car RemoveCar(){
            if(lastIndex == -1){
                return null;
            }
            else{
                return cars[lastIndex--];
            }
        }
    }
}
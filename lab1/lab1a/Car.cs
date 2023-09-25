using System;

namespace lab1a{
    public class Car{
        public string Model {get; set;}
        public int Year {get; set;}
        public double EngineVolume {get; set;}

        public Car(string model, int year, double engineVolume){
            Model = model;
            Year = year;
            EngineVolume = engineVolume;
        }
        public string SomeCalc(){
            return "A";
        }
    }

}
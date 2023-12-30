#!/usr/bin/env python3
import os
import argparse
import json
import random
parser = argparse.ArgumentParser(description='Generate cars data')

parser.add_argument('count', type=int, help='Data count')
parser.add_argument('file_name', type=str, help='Output file')
parser.add_argument('match', type=str, help='Matching criteria (even)')
args = parser.parse_args()

car_list = [
    "Toyota Corolla",
    "Honda Accord",
    "Ford Mustang",
    "Chevrolet Malibu",
    "Volkswagen Jetta",
    "BMW 3 Series",
    "Mercedes-Benz C-Class",
    "Nissan Altima",
    "Hyundai Sonata",
    "Kia Optima",
    "Audi A4",
    "Mazda3",
    "Subaru Outback",
    "Lexus RX",
    "Tesla Model 3",
    "Jeep Wrangler",
    "Ford F-150",
    "Chevrolet Silverado",
    "Toyota Camry",
    "Honda Civic",
    "Volkswagen Passat",
    "Hyundai Elantra",
    "Nissan Rogue",
    "Volvo XC90",
    "Acura MDX",
    "Buick Encore",
    "Cadillac Escalade",
    "Dodge Charger",
    "Fiat 500",
    "GMC Sierra",
    "Infiniti Q50",
    "Jaguar F-Type",
    "Kia Soul",
    "Land Rover Discovery",
    "Lamborghini Huracan",
    "Maserati Ghibli",
    "Mazda CX-5",
    "Mercedes-Benz GLC",
    "Mini Cooper",
    "Mitsubishi Outlander",
    "Nissan Murano",
    "Porsche 911",
    "Ram 1500",
    "Subaru Forester",
    "Tesla Model S",
    "Toyota Highlander",
    "Volkswagen Tiguan",
    "Volvo S90",
    "Audi Q7",
    "Chevrolet Equinox",
    "Dodge Journey",
    "Ford Escape",
    "Genesis G70",
    "Honda Pilot",
    "Infiniti QX60",
    "Jeep Grand Cherokee",
    "Kia Sorento",
    "Lexus GX",
    "Mazda MX-5 Miata",
    "Mercedes-Benz GLE",
    "Nissan Pathfinder",
    "Porsche Cayenne",
    "Subaru Crosstrek",
    "Tesla Model X",
    "Toyota RAV4",
    "Volkswagen Golf",
    "Acura RDX",
    "BMW X5",
    "Chevrolet Traverse",
    "Dodge Durango",
    "Ford Explorer",
    "GMC Acadia",
    "Honda HR-V",
    "Infiniti QX80",
    "Jaguar E-PACE",
    "Kia Telluride",
    "Lexus NX",
    "Maserati Levante",
    "Mercedes-Benz A-Class",
    "Nissan Sentra",
    "Porsche Macan",
    "Ram 2500",
    "Subaru Ascent",
    "Tesla Model Y",
    "Toyota Tacoma",
    "Volkswagen Atlas",
    "Volvo XC40"
]

generated_data = []
file_path = "{0}/{1}".format(os.path.dirname(os.path.realpath(__file__)),args.file_name)
if os.path.isfile(file_path):
    os.remove(file_path)

n = args.count
if len(car_list) < n:   
    n = len(car_list)


for i in range(args.count):
    year = random.randint(2000, 2020)
    volume = (float)(random.randint(1,3)) + random.choice([0, 0.5, 0.6])
   
    value = len(car_list[i]) + (int)(volume) + year
    if value % 2 != 0:
        year = year + 1

    generated_data.append({'model':car_list[i], 'year': year, 'enginevolume': volume})


if args.match == 'none':
    print("none")
    for i in range(n):
        generated_data[i]['year'] = generated_data[i]['year'] - 1;
       
if args.match == 'half':
    print("half")
    count = args.count / 2
    modified = 0
    for i in range(n):
        if random.randint(0, 1) == 1:
            generated_data[i]['year'] =  generated_data[i]['year'] - 1
            modified = modified + 1
            if modified == count:
                break;

    print("first modification count ", modified)
    i = 0
    if modified != count:
        for i in range(n):
            entry = generated_data[i]
            value = len(entry['model']) + entry['year'] + entry['enginevolume'] 
            if value % 2 == 0:
                generated_data[i]['year'] =  generated_data[i]['year'] - 1
                modified = modified + 1
            if modified == count:
                break
    print("after modification ", modified)
        # print(len(generated_data[i]['model']) + (int)(generated_data[i]['enginevolume']) * 1000 + generated_data[i]['year'])



jsondict = json.dumps(generated_data)

with open(file_path, 'w') as out:
    json_str = json.dumps(jsondict)
    json_str = str(json_str).replace("\\", "")
    json_str = json_str[1:len(json_str) - 1] 
  
    out.write(json_str)
    out.close()
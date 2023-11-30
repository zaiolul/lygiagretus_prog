#!/usr/bin/python

import os
import argparse
import json
import random
import numpy as np

def distance(point1, point2):
    return np.sqrt((point1[0] - point2[0]) ** 2 + (point1[1]- point2[1])**2)

parser = argparse.ArgumentParser(description='Data point generation')

parser.add_argument('-b', type=int, help='Existing count', required=True)
parser.add_argument('-n', type=int, help='New count', required=True)
parser.add_argument('-o', type=str, help='Output file name (creates in script dir)', required=True)
args = parser.parse_args()


file_path = "{0}/{1}".format(os.path.dirname(os.path.realpath(__file__)),args.o)
if os.path.isfile(file_path):
    os.remove(file_path)

limit = 10 
data_base = np.random.uniform(-limit, limit, (args.b, 2)).tolist()
# print(data_base)
eps = 1e-2
data_new = []
retry = False;
for i in range(args.n):
    point = np.random.uniform(-limit, limit,(1, 2))
    retry = False
    for j in range(args.b):
        if distance(point[0], data_base[j]) < eps:
            # print("RETRY")
            retry = True
            break
    if retry:
        i -= 1;
        continue;
    print(point[0])
    data_new.append([point[0][0], point[0][1]])


data = dict({"base" : data_base, "new" : data_new})
jsondict = json.dumps(data)
# print(data)
print(jsondict)

with open(file_path, 'w') as out:
    json_str = json.dumps(jsondict)
    json_str = str(json_str).replace("\\", "")
    json_str = json_str[1:len(json_str) - 1] 
  
    out.write(json_str)
    out.close()
        








#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import argparse as arg
import json
import os
from multiprocessing import Pool

def draw(base, new, limit):
   
    plt.plot(base[:,0],base[:,1], '.r', label='Esamos parduotuvės', linestyle = 'None')
    plt.plot(new[:,0],new[:,1], '.b',   label='Naujos parduotuvės', linestyle = 'None')
   
    plt.xlim([-limit-2, limit+2])
    plt.ylim([-limit-2, limit+2])
    plt.plot([-limit, -limit, limit, limit, -limit], [-limit, limit, limit, -limit, -limit],'--k') 
    plt.grid()
    plt.legend()
    plt.show()

def distance(pos1, pos2):
    x1 = pos1[0]
    x2 = pos2[0]
    y1 = pos1[1]
    y2 = pos2[1]

    return np.exp(-0.3 *((x1 - x2)**2 + (y1 - y2)**2))

def loc_price(pos):
    x1 = pos[0]
    y1 = pos[1]

    return ((x1**4 + y1**4) / 1000) + ((np.sin(x1) + np.cos(y1)) / 5) + 0.4

def _target(args):
    base = args[0]
    new = args[1]
    current = args[2]


    n_base = len(base)
    n_new = len(new)
    sum = loc_price(new[current])
    for i in range(n_base):
        sum += distance(new[current], base[i])
            

    for i in range(n_new):
        if i == current:
            continue
        sum += distance(new[current], new[i])
    return sum



def target(base, new, pool):
    args_list = [(base, new, i) for i,_ in enumerate(new)]
    res = pool.imap(_target, args_list, 3)
    return sum(res)
    

def numerical_gradient(base, new, h, pool):
    f0 = target(base, new, pool)
    G = np.zeros(shape=(len(new), 2))
    for i in range(len(new)):
        for j in range (2): 
            temp = np.copy(new)
            temp[i,j] += h
            f1 = target(base, temp, pool)
            G[i, j] = (f1 - f0) / h;
    norm = np.linalg.norm(G)
    G = G / norm

    return G 

def parse_args():
    parser = arg.ArgumentParser(prog="lp_proj",
                              description="LP projektas",
                              usage="./lp_proj.py [-p PROCESSES] -i INPUT_FILE -o OUTPUT_FILE")
    parser.add_argument("-p", default=4,
                      help="Process count (default 4)", type=int)
    parser.add_argument("-i", required=True,
                      help="Input data file", type=str)
    # parser.add_argument("-o", required=True,
    #                   help="Output file", type=str)
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_args()
    eps = 1e-8
    limit = 10 # n x n area
    file_path = "{0}/data/{1}".format(os.path.dirname(os.path.realpath(__file__)),args.i)
    file = open(file_path)
    data = json.load(file)
    
    # print("base:\n", data["base"])
    # print("new:\n", data["new"])
  
    
    base_shops = np.array(data["base"])
    new_shops = np.array(data["new"])

    np.random.seed(11)

    # n_base = 2

    # n_new = 2

    # base_shops = np.random.uniform(-10, 10, (n_base, 2)) 
    # new_shops = np.random.uniform(-10, 10, (n_new, 2)) 

    pool = Pool(args.p)
  
    # draw(base_shops, new_shops, limit)

    itmax = 1000
    step = 0.2

    h = 0.001

    fx = target(base_shops, new_shops, pool)

    f_vals = []
  
    for i in range(itmax):
        G = numerical_gradient(base_shops, new_shops, h, pool)
        new_shops -=  step * G
        fx1 = target(base_shops, new_shops, pool)
        # f_vals.append(fx1)
        
        if fx1 > fx:
            new_shops +=  step * G
            step = step * 0.9
            # print('fx1 = ', fx1, 'step = ', step, 'i =', i)
        
        else:
            fx = fx1

        if step < eps:
            # print("optimizavimas baigtas, fx =", fx, ",iteraciju skaicius:", i )
            break

    # draw(base_shops, new_shops, limit)
    # print(new_shops)
    pool.close()
    pool.join()
# plt.plot(its, f_vals)
# plt.xlabel("iteracijos")
# plt.ylabel("tikslo funkcijos reikšmė")
# plt.show()
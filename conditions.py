#! /usr/bin/python2

import json
import numpy as np
import random

Cond = []
niche_A = np.logspace(-2, 0, num = 19)
niche_R = np.logspace(-2, 0, num = 19)
for ea in niche_A:
    for er in niche_R:
           for replicate in xrange(10):
              Cond.append(json.dumps({'bP': ea, 'bG': er})+'\n')

random.shuffle(Cond);

with open('conditions.json', 'w') as fp:
    fp.writelines(Cond)


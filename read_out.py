import json
import scipy as sp
import numpy as np

import glob
import os

def Span(v):
    return np.max(v) - np.min(v)

def summarizeSimul(S):
    pa = S['params']
    steps = S['steps']
    ## Find the last step
    last_time = np.max([int(s['t']) for s in steps])
    step = [s for s in steps if int(s['t']) == last_time][0]
    ## Get some summary stats
    pa['Time'] = last_time
    pa['PreyBiomass'] = np.sum([sp['n'] for sp in step['preys']])
    pa['PredBiomass'] = np.sum([sp['n'] for sp in step['preds']])
    pa['PreyRichness'] = len(step['preys'])
    pa['PredRichness'] = len(step['preds'])
    pa['PreyTraitSpan'] = Span([sp['x'] for sp in step['preys']])
    pa['PredTraitSpan'] = Span([sp['x'] for sp in step['preds']])
    pa['PreyMeanAge'] = np.mean([last_time-int(sp['b']) for sp in step['preys']])
    pa['PredMeanAge'] = np.mean([last_time-int(sp['b']) for sp in step['preds']])
    return pa

ArrayOfSummary = []

# Read all the files and get the results for the last time point
for json_file in glob.glob("outputs/*.json"):
    with open(json_file, 'r') as json_string:
        try :
            sim = json.load(json_string)
        except ValueError :
            pass
        finally :
            withStats = summarizeSimul(sim)
            ArrayOfSummary.append(withStats)

with open('results.json', 'w') as outfile:
    json.dump(ArrayOfSummary, outfile)

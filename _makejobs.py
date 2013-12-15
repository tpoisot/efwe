#!/opt/python27/bin/python

import json
import numpy as np
import scipy as sp

def create_script(fline, er, ep):
    f = open("job%(fl_)d.sh" % {'fl_' : fline}, 'w')
    f.write("#---------------------------Start program.job------------------------\n")
    f.write("#!/bin/bash\n")
    f.write("\n")
    f.write("# The name of the job, can be whatever makes sense to you\n")
    f.write("#$ -N e%(id_)d\n" % {"id_" : fline})
    f.write("\n")
    f.write("# Redirect output stream to this file.\n")
    f.write("#$ -o evo-Yeah-ID%(id_)d\n" % {"id_" : fline})
    f.write("\n")
    f.write("# Redirect error stream to this file.\n")
    f.write("#$ -e evo-Fuck-ID%(id_)d\n" % {"id_" : fline})
    f.write("\n")
    f.write("# The batchsystem should use the current directory as working directory.\n")
    f.write("# Both files (output.dat and error.dat) will be placed in the current\n")
    f.write("# directory. The batchsystem assumes to find the executable in this directory.\n")
    f.write("#$ -cwd\n")
    f.write("\n")
    f.write("# This is the file to be executed.\n")
    f.write("./efwe --id=%(id_)d --bG=%(ngrow_).9f --bP=%(npred_).9f --time=50000 \n" % {"id_" : fline, "ngrow_": er, "npred_": ep})
    f.write("\n")
    f.write("#---------------------------End program.sge------------------------\n")
    f.close()

num_lines = sum(1 for line in open('conditions.json'))
fp = open('conditions.json')
ids = [i for i in xrange(num_lines)]
for i in ids:
    for nl, line in enumerate(fp):
        if nl == i:
            Pa = json.loads(line)
    create_script(i, Pa['bG'], Pa['bP'])


f = open("run.sh", 'w')
f.write("#---------------------------Start script------------------------\n")
f.write("#!/bin/bash\n")
f.write("\n")
f.write("# The SGE jobs to be executed.\n")
f.write("for ((  i = 0 ;  i < %(sim)d;  ++i  ))\n" % {"sim" : np.max(ids)+1})
f.write("do\n")
f.write("  qsub -q all.q@noeud01.local,all.q@noeud02.local.all.q@noeud03.local job$i.sh\n")
f.write("done\n")
f.write("\n")
f.write("#---------------------------End script------------------------\n")
f.close()

# Use: ./gen_data.py <input_length_desired> <output_file_name>

import random
import sys
import numpy as np

if len(sys.argv) != 3:
    print ("Usage: ", sys.argv[0], " <length> <output_file>")
    exit()

#output file
f = open(sys.argv[2], 'w')

#length of the list needed
l = sys.argv[1]

f.write(l + "\n")
l = int(l)

array = []

for i in range(l):
    r = random.randrange(100000)
    f.write(str(r) + " ")
    array.append(r)

f.close()

np.save('input.npy', array)
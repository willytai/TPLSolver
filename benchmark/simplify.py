import sys, os

inputfile = sys.argv[1]
outputfile = open(os.path.join('simplified_benchmark', inputfile), 'w')

with open(inputfile, 'r') as f:
    for line in f:
        line = line.strip()
        if line[:2] != 'XY':
            continue
        outputfile.write('{}\n'.format(line))
outputfile.close()

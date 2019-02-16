import sys, os, numpy

inputfile = sys.argv[1]
outputfile = inputfile.split('/')[-1].replace('.ascii', '.txt')
if not os.path.isdir('txt'):
    os.mkdir('txt')
outputfile = os.path.join('txt', outputfile)
outfile = open(outputfile, 'w')

print ('reading ', inputfile)
print ('saveing to ', outputfile)

polygon_count = 0;

with open(inputfile, 'r') as f:
    for line in f:
        line = line.strip()
        line = line[4:]
        line = line.split(', ')
        line = numpy.array(line).reshape((len(line)//2, 2))

        for data in line:
            outfile.write('{} {}\n'.format(data[0], data[1]))
        outfile.write('\n')

        polygon_count += 1

print ('Total Polygons :', polygon_count)

outfile.close()

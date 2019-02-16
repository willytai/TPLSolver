import sys

infilename = sys.argv[1]

outfilename = infilename+'_adjlist'

adjlist = dict()

with open(infilename, 'r') as f:
    for line in f:
        line = line.strip().split()
        if line[0] == 'vertex': continue
        if line[0] == 'c': continue
        v1 = int(line[0]) - 1
        v2 = int(line[1]) - 1
        if v1 not in adjlist: adjlist[v1] = [v2]
        else: adjlist[v1].append(v2)
        if v2 not in adjlist: adjlist[v2] = [v1]
        else: adjlist[v2].append(v1)

sorted_list = sorted(adjlist.items(), key=lambda x: x[0])
print (sorted_list)

with open(outfilename, 'w') as f:
    for data in sorted_list:
        f.write('{}\n'.format(' '.join(list(map(str, data[1])))))

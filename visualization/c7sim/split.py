import sys
if len(sys.argv) != 2:
    print ('Usage: python3 split.py c*_report.txt')
    exit()
report_file = sys.argv[1]

conflict = 'c7_conflict.txt'
c7_r = 'c7_red.txt'
c7_g = 'c7_blue.txt'
c7_b = 'c7_green.txt'

f_conflict = open(conflict, 'w')
f_r = open(c7_r, 'w')
f_g = open(c7_g, 'w')
f_b = open(c7_b, 'w')

ID_color_map = {}
with open(report_file, 'r') as f:
    conflict_v_collected = False
    for line in f:
        line = line.strip().split(':')
        if line[0] == 'conflict vertices':
            conflict_v_collected = True
            conflict_list = list(map(int, line[-1].strip().split(' ')))
            continue
        if conflict_v_collected:
            line = line[0].split(',')
            ID_color_map[int(line[1])] = int(line[3])

poly_ID_map = {}
with open('c7sim_polygon_with_ID.txt', 'r') as f:
    for line in f:
        line = line.strip()
        if len(line) == 0: continue
        line = line.split(' ')
        if line[0] == 'ID:':
            current_ID = int(line[1])
            poly_ID_map[int(line[1])] = list()
        else:
            poly_ID_map[current_ID].append((int(line[0]), int(line[1])))

for ID, poly in poly_ID_map.items():
    if ID in conflict_list:
        target = f_conflict
    elif ID_color_map[ID] == 1:
        target = f_r
    elif ID_color_map[ID] == 2:
        target = f_g
    elif ID_color_map[ID] == 3:
        target = f_b
    else:
        assert False

    for coor in poly:
        target.write('{} {}\n'.format(coor[0], coor[1]))
    target.write('\n')

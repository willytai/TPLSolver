# find the mapping of the polygons
# example: python3 c1sim_poly_ID_mapping c1sim_merged
#                  <mapping of bbox>     <original polygon>

import sys

if len(sys.argv) != 3:
    print ('Usage: python3 <mapping of bbox> <original polygon>')
    exit()

rect_patt = sys.argv[1]
rect_dict = {}

output_file = open('{}_polygon_with_ID.txt'.format(rect_patt.split('_')[0]), 'w')

with open(rect_patt, 'r') as f:
    ID = None
    for line in f:
        line = line.strip()
        if line[:2] == 'ID':
            ID = int(line.split(' ')[-1])
        else:
            line = list(map(int, line[1:-1].split(',')))
            rect_dict[ID] = ((line[0], line[1]), (line[2], line[3]))

poly_patt = sys.argv[2]
polygons = []

with open(poly_patt, 'r') as f:
    coor = []
    for line in f:
        line = line.strip()
        if len(line) == 0:
            polygons.append(coor[:-1])
            coor = []
            continue
        coor.append(tuple(map(int, line.split())))
if len(coor) != 0: polygons.append(coor)


def poly_in_bbox(poly, bbox):
    lx, ly = bbox[0]
    rx, ry = bbox[1]
    on_boundary_count = 0
    for i, coor in enumerate(poly):
        x, y = coor
        if not (lx <= x and x <= rx and ly <= y and y <= ry): return False
        if lx == x or x == rx or ly == y or y == ry: on_boundary_count += 1
    if on_boundary_count < 4: return False
    return True


possible_mapping = {}

for ID, bbox in rect_dict.items():
    for poly in polygons:
        if poly_in_bbox(poly, bbox):
            if ID not in possible_mapping:
                possible_mapping[ID] = [poly]
            else:
                possible_mapping[ID].append(poly)
            polygons.remove(poly)


for ID, poly in possible_mapping.items():
    if len(poly) != 1:
        print (ID)
        print ('original bbox')
        print (rect_dict[ID])
        for fuck in poly: print (fuck)


for ID, poly in possible_mapping.items():
    assert len(poly) == 1
    output_file.write('ID: '+str(ID)+'\n')
    for coor in poly[0]:
        output_file.write('{} {}\n'.format(coor[0], coor[1]))
    output_file.write('{} {}\n\n'.format(poly[0][0][0], poly[0][0][1]))


















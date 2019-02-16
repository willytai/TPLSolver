import pydot, sys
import random

filename = sys.argv[1]

vertex_Node = dict()
edges = dict()
uncolorable = dict()

with open(filename, 'r') as f:
    for line in f:
        if line[:6] == 'vertex':
            line = line.strip().split(',')
            if line[-1] == '1': color = "red"
            elif line[-1] == '2': color = "green"
            elif line[-1] == '3': color = "blue"
            else: assert False
            vertex_id = int(line[1])
            if vertex_id in uncolorable: color = uncolorable[vertex_id]
            vertex_Node[vertex_id] = pydot.Node("{}".format(vertex_id), style="filled", fillcolor=color)
        elif line[2] == 'V':
            line = line.strip().split()
            line = list(filter(lambda x: x != 'V', line))
            line = list(filter(lambda x: x != '|', line))
            line = [int(x[:-1]) for x in line]
            r = random.randrange(20, 100)
            g = random.randrange(30, 100)
            b = random.randrange(10, 100)
            for x in line: 
                if x not in uncolorable:
                    uncolorable[x] = "#"+str(r)+str(g)+str(b)
                else:
                    p_r = int(uncolorable[x][1:3])
                    p_g = int(uncolorable[x][3:5])
                    p_b = int(uncolorable[x][5:7])
                    r = (r + p_r) // 2
                    g = (g + p_g) // 2
                    b = (b + p_b) // 2
                    uncolorable[x] = "#"+str(r)+str(g)+str(b)
        else:
            line = line.strip().split();
            if line[0][-1] != ":": continue
            current_v = int(line[0][:-1])
            adj_vertex = list(map(int, line[1:]))
            for adj_v in adj_vertex:
                if adj_v > current_v: e = (adj_v, current_v)
                else: e = (current_v, adj_v)
                if e not in edges: edges[e] = True


G = pydot.Dot(graph_type='graph')

for ID, Node in vertex_Node.items():
    G.add_node(Node)

for e, v in edges.items():
    G.add_edge(pydot.Edge(vertex_Node[e[0]], vertex_Node[e[1]]))

G.write_png(filename+'.png')

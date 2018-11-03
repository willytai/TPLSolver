#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

struct Vertex
{
    Vertex(int id) { ID = id; }
    int ID;
    vector<Vertex*> VertexList;

    // overload cout
    friend ostream& operator << (ostream& os, const Vertex& v) {
        os << v.ID;
        return os;
    }
};

struct Edge
{
    Edge(Vertex* vv1, Vertex* vv2) { v1 = vv1; v2 = vv2; }
    Vertex* v1;
    Vertex* v2;

    // overload cout
    friend ostream& operator << (ostream& os, const Edge& e) {
        os << '(' << *(e.v1) << ", " << *(e.v2) << ')';
        return os;
    }
};

class Graph
{
public:
    Graph() {}
    ~Graph() {}

    void ContstructByFile (fstream& file);
    void RetrieveVertexes (vector<Vertex*>& vec);
    void RetrieveEdges    (vector<Edge*>& vec);

private:
    vector<Vertex*>     _vertex;
    vector<Edge*>       _edge;

    void add_vertex     (istringstream& iss);
    void construct_edge (string& token, istringstream& iss);
};

#endif /* GRAPH_H */

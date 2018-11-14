#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stack>
#include <cassert>
#include "Vertex.h"
#include "Edge.h"

using namespace std;

class Graph
{
    friend class UncolorableSubgraphIdentifier;
public:
    Graph() : _bfsDone(false) {}
    ~Graph() {}

    void ContstructByFile        (fstream& file);
    void RetrieveVertexes        (vector<Vertex*>& vec)          const;
    void RetrieveEdges           (vector<Edge*>& vec)            const;
    void GetBFSList              (vector<Vertex*>& vec)          const;
    void RemoveEdge              (Edge* e);
    void RemoveVertex            (Vertex* v);
    void RecoverVertex           (Vertex* v);
    void RemoveVertex            (int idx);
    void RecoverVertex           (int idx);
    void bfs                     ();
    void print                   ()                              const;
    int  get_order               (const Vertex* v)               const;
    int  size                    ()                              const;

    Vertex* operator [] (int idx) { return _bfsList[idx]; }

private:
    vector<Vertex*>     _vertex;
    vector<Vertex*>     _bfsList;
    vector<Edge*>       _edge;
    stack<Edge*>        _removed_edges;
    stack<Vertex*>      _removed_vertexes;

    mutable bool        _bfsDone;

    void add_vertex     (istringstream& iss);
    void construct_edge (string& token, istringstream& iss);
    void restore        (Vertex* v);
};

#endif /* GRAPH_H */

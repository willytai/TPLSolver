#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stack>
#include <map>
#include <cassert>
#include "Vertex.h"
#include "Edge.h"
#include "Cell.h"

using namespace std;


const Color COLORS[4] = {UNDEF, RED, GREEN, BLUE};

enum CheckState { RETURN_FALSE, RETURN_TRUE, KEEP_SEARCHING };


class Graph
{
public:
    Graph() : _root(NULL), _bfsDone(false) {}
    ~Graph() {}

    void   ContstructByFile (fstream& file);
    void   RetrieveVertexes (vector<Vertex*>& vec) const;
    void   RetrieveEdges    (map<int, map<int, Edge*> >& vec) const;
    void   GetBFSList       (vector<Vertex*>& vec) const;
    void   RemoveEdge       (int id_1, int id_2);
    void   RemoveVertex     (Vertex* v);
    void   RecoverVertex    (Vertex* v);
    void   RemoveVertex     (int idx);
    void   RecoverVertex    (int idx);
    void   bfs              ();
    void   print            ();
    int    get_order        (const Vertex* v)      const;
    size_t size             ()                     const;


    /*******************************************************
    * this part is for uncolorable subgraph identification *
    *******************************************************/
    void              ApplySolution     (const vector<Cell*>& sol, int RootID);
    void              GetConflictEdges  (vector<pair<int, int> >& cedges);
    void              runIdentification ();
    bool              propagate         (Vertex* currentVertex);
    inline CheckState check             (Vertex*& currentVertex);
    

    Vertex* operator [] (int idx) { return _bfsList[idx]; }

private:
    vector<vector<int> >       _adjList;            // adjacency list
    vector<Vertex*>            _vertex;
    vector<Vertex*>            _bfsList;
    map<int, map<int, Edge*> > _edge;               // _edge[1][2] is the edge pointer formed by vertex 1 and 2, and is only record once
    stack<Edge*>               _removed_edges;
    stack<Vertex*>             _removed_vertexes;

    Vertex*                 _root;                  // this is for uncolorable subgraph identification
    vector<pair<int, int> > _conflict_edges;        // to store the conflict edges

    mutable bool          _bfsDone;

    void add_vertex     (istringstream& iss);
    void construct_edge (string& token, istringstream& iss);
    void restore        (Vertex* v);
};

#endif /* GRAPH_H */

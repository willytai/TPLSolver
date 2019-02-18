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

class Graph
{
public:
    Graph() : _root(NULL), _bfsDone(false) {}
    ~Graph() {}

    void   ContstructByFile    (fstream& file);
    void   ContstructByAdjList (fstream& file);
    void   RetrieveVertexes    (vector<Vertex*>& vec, const int& component_id) const;
    void   RetrieveEdges       (map<int, map<int, Edge*> >& vec, const int& component_id) const;
    void   RemoveEdge          (int id_1, int id_2, int component_id);
    void   RestoreEdges        ();
    void   RemoveVertex        (Vertex* v);
    void   RecoverVertex       (Vertex* v);
    void   RemoveVertex        (int idx);
    void   RecoverVertex       (int idx);
    void   GetBFSList          (vector<Vertex*>& vec, int component_id) const;
    void   GetConflictID       (vector<int>& conflict_vID);
    void   print               ();
    int    bfs                 (int component_id, int r_id);
    int    get_order           (const Vertex* v)      const;
    size_t size                (const int& component_id) const;
    size_t numComponents       () const;

    Vertex* GetVByBFSIndex     (int component_id, int idx) { assert(size_t(idx) < _bfsList[component_id].size()); return _bfsList[component_id][idx]; }


    /*******************************************************
    * this part is for uncolorable subgraph identification *
    *******************************************************/
    void              ApplySolution           (const vector<Cell*>& sol, int RootID);
    void              GetLatestConflictEdges  (vector<pair<int, int> >& cedges);
    void              reportConflictSubgraphs (ostream& os, string filename) const;
    void              runIdentification       (const int& component_id);
    void              RemoveEdges             (const int& component_id);
    bool              propagate               (Vertex* currentVertex);
    inline bool       check                   (Vertex*& currentVertex, vector<bool>& LegalColor);

    // this part is for graph visualization only
    void write_adjlist(ostream& os, vector<Cell*>& sol);

private:
    vector<vector<int> >                _adjList;      // adjacency list
    vector<int>                         _root_of_cc;   // roots of connected components
    vector<Vertex*>                     _vertex;
    vector<vector<Vertex*> >            _bfsList;      // bfslist of each CC
    vector<map<int, map<int, Edge*> > > _edge;         // _edge[1][2] is the edge pointer formed by vertex 1 and 2, and is only record once
    stack<Edge*>                        _removed_edges;
    stack<Vertex*>                      _removed_vertexes;

    vector<pair<int, int> >             _latest_conflict_edges;        // to store the conflict edges
    vector<vector<Vertex*> >            _conflict_subgraphs;           // to store the conflict edges
    vector<Vertex*>                     _temporary_colorable_vertexes;
    Vertex*                             _root;                         // this is for uncolorable subgraph identification
    int*                                _predecessor;                  // for identifing CC

    mutable bool          _bfsDone;

    void add_vertex                   (istringstream& iss);
    void construct_edge               (string& token, istringstream& iss);
    void restore                      (Vertex* v);
    void identify_connected_component ();
};

#endif /* GRAPH_H */

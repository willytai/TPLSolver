#ifndef UNCOLORABLE_SUBGRAPH_IDENTIFIER_H
#define UNCOLORABLE_SUBGRAPH_IDENTIFIER_H

#include "DLX_struct.h"

const Color COLORS[4] = {UNDEF, RED, GREEN, BLUE};

class UncolorableSubgraphIdentifier
{
    friend class Graph;
public:
    UncolorableSubgraphIdentifier(vector<Vertex*>& ColoredVertexes, int MaxID, Edge* ConflictEdge);
    UncolorableSubgraphIdentifier() {}
    ~UncolorableSubgraphIdentifier() {}

    void InitByColoredVertexes(const vector<Cell*>& sol, int RootID, int VertexNum);
    void bfs();
    void run();

private:
    vector<Vertex*>     _vertex;
    Vertex*             _root;

    bool propagate(Vertex* currentVertex);
};

#endif /* UNCOLORABLE_SUBGRAPH_IDENTIFIER-H */

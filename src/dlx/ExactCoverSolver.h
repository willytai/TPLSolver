#ifndef EXACT_COVER_SOLVER_H
#define EXACT_COVER_SOLVER_H

#include "DLX_struct.h"
#include <stack>

class ExactCoverSolver
{
public:
    ExactCoverSolver() {}
    ~ExactCoverSolver() {}

    void InitByFile  (fstream& file);
    void InitByGraph (Graph& g);
    void Solve       ();

private:
    DancingLink     _dlx;
    Graph           _graph;
    vector<Cell*>   _solution;

    bool  X_star               (int bfsIndex);
    void  CoverAffectedCells   (const Cell*, stack<Cell*>&);
    void  UNCoverAffectedCells (stack<Cell*>&);
    Cell* FindPriorityColumn   (const Cell* header);
};

#endif /* EXACT_COVER_SOLVER_H */

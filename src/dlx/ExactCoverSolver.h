#ifndef EXACT_COVER_SOLVER_H
#define EXACT_COVER_SOLVER_H

#include "DLX_struct.h"
#include <stack>

enum SolverState
{
    SUCCESS,
    CONFLICT_NOT_DONE,
    CONFLICT_BUT_DONE,
    STATE_UNDEFINED
};

class ExactCoverSolver
{
public:
    ExactCoverSolver() : _component_id(0) {}
    ~ExactCoverSolver() {}

    void InitByFile    (fstream& file);
    void InitByAdjList (fstream& file);
    void InitByGraph   (Graph& g);
    void Solve         ();
    void report        (ostream& os, string filename);

private:
    DancingLink     _dlx;
    Graph           _graph;
    vector<Cell*>   _solution;
    int             _component_id;

    SolverState X_star (int bfsIndex, bool recordPartialResult);

    Cell* FindPriorityColumn   (const Cell* header);
    void  solve                (int component_id);
    void  CoverAffectedCells   (const Cell*, stack<Cell*>&);
    void  UNCoverAffectedCells (stack<Cell*>&);
    void  IdentifyUncolorablePartAndRemove();
};

#endif /* EXACT_COVER_SOLVER_H */

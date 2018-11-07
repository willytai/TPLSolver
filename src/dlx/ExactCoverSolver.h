#ifndef EXACT_COVER_SOLVER_H
#define EXACT_COVER_SOLVER_H

#include "DLX_struct.h"
#include <stack>

/*! \enum SolverState
 *
 *  SUCCESS -> the problem is three-colorable
 *
 */
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
    ExactCoverSolver() {}
    ~ExactCoverSolver() {}

    void InitByFile  (fstream& file);
    void InitByGraph (Graph& g);
    void Solve       ();

private:
    DancingLink     _dlx;
    Graph           _graph;
    vector<Cell*>   _solution;

    SolverState X_star (int bfsIndex, bool recordPartialResult);

    Cell* FindPriorityColumn   (const Cell* header);
    void  CoverAffectedCells   (const Cell*, stack<Cell*>&);
    void  UNCoverAffectedCells (stack<Cell*>&);
};

#endif /* EXACT_COVER_SOLVER_H */

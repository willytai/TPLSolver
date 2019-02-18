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
    void report        (ostream& os, string filename, bool dump_adjlist);

private:
    DancingLink              _dlx;
    Graph                    _graph;
    vector<vector<Cell*> >   _solution;      // _solution[i] represnets the solution of the ith connected componet
    int                      _component_id;

#ifdef ENGINE_V1
    SolverState X_star (int bfsIndex, bool recordPartialResult, const int Depth);
#endif
#ifdef ENGINE_V2
    SolverState X_star (int bfsIndex, bool recordPartialResult, int Depth);
#endif

    Cell* FindPriorityColumn      (const Cell* header);
    void  solve                   (int component_id);
    void  CoverAffectedCells      (const Cell*, stack<Cell*>&);
    void  UNCoverAffectedCells    (stack<Cell*>&);
    void  IdentifyUncolorablePartAndRemove();

#ifdef DEBUG_XSTAR
    void print() const;
#endif
};

#endif /* EXACT_COVER_SOLVER_H */

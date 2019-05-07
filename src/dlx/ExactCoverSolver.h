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
#define NATIVE_CONFLICT_DETECTION_ENABLED 1
#define NATIVE_CONFLICT_DETECTION_DISABLED 0
public:
    ExactCoverSolver(string s) : _component_id(0) { this->initmode(s);  }
    ~ExactCoverSolver() {}

    void InitByFile    (fstream& file);
    void InitByAdjList (fstream& file);
    void InitByGraph   (Graph& g);
    void Solve         ();
    void report        (ostream& os, string filename, bool dump_adjlist);

    void initmode(string s) {
        if (s == "enable") {
            _detection_stat = NATIVE_CONFLICT_DETECTION_ENABLED;
        }
        else if (s == "disable") {
            _detection_stat = NATIVE_CONFLICT_DETECTION_DISABLED;
        }
        else {
            cerr << "Invalid option for detection : " << s << endl;
            cerr << "Use enable/disable" << endl;
            exit(0);
        }
    }

private:
    DancingLink              _dlx;
    Graph                    _graph;
    vector<vector<Cell*> >   _solution;      // _solution[i] represnets the solution of the ith connected componet
    int                      _component_id;
    int                      _detection_stat;

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

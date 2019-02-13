#ifdef ENGINE_V1

#include "ExactCoverSolver.h"
#include <iomanip>

void ExactCoverSolver::solve(int component_id) {

    _dlx.clear();
    _dlx.init(_graph, component_id);

    _solution[component_id].clear();
    cerr << "Running X_star...";
    SolverState result = X_star(0, true, 0);
    cerr << "Done" << endl;

    while (result != SUCCESS) {
#ifdef DEBUG_MODE_SOL
        cout << endl << "solution:" << endl;
        for (auto it = _solution[component_id].begin(); it != _solution[component_id].end(); ++it)
            cout << *(*it) << endl;
#endif
        cout << "Conflict found, need to remove and keep on searching" << endl;

        this->IdentifyUncolorablePartAndRemove();
        // this->ApplySolution();
        _solution[component_id].clear();

        // _dlx.clear();
        // _dlx.init(_graph, component_id);

        cerr << "Running X_star...";
        result = X_star(0, true, 0);
        cerr << "Done" << endl;
    }

#ifdef DEBUG_MODE_SOL
    cout << endl << "solution:" << endl;
    for (auto it = _solution[component_id].begin(); it != _solution[component_id].end(); ++it)
        cout << *(*it) << endl;
#endif
    if (result == SUCCESS) cout << "Success!" << endl << endl;
}


SolverState ExactCoverSolver::X_star(int bfsIndex, bool recordPartialResult, const int Depth) {
    if (_dlx.isGoal()) return SUCCESS;

    /*******************************/
    /* cover vertex with bfs order */
    /*******************************/
#ifdef ENABLE_PRIORITY_VERTEX
    // cover target column
    // check if there is any vertex that can be colored (only one color choice remains)
    // column with only one NormalCell
    Cell* PriorityColumnCell = FindPriorityColumn(_dlx.GetHeader());
#else
    Cell* PriorityColumnCell = NULL;
    assert(0);
#endif

    // if this happens, it means that after some edges removed by the identifier, some isolated vertex appears
    // therefore, there will be no need to consider the order of the vertex traversed after this point
    // just simply try to cover the column to the right of the _dlx header
    if (size_t(bfsIndex) > _graph.size(_component_id)-1) {
        PriorityColumnCell = _dlx.GetHeader()->right;
#ifdef DEBUG_MODE_EXACTSOLVER
        cout << "Toggling isolated vertex, ID: " << PriorityColumnCell->GetCorrespondVertex()->ID << endl;
#endif
    }

    // otherwise, cover with bfs order
    Cell* TargetColumnCell = PriorityColumnCell;
    if (!PriorityColumnCell) {
        TargetColumnCell = _dlx.Column(_graph.GetVByBFSIndex(_component_id, bfsIndex)->ID);
        assert(TargetColumnCell);
    }
    // maintain bfs order
    else --bfsIndex;


    // if this is false, it means that this column is covered preivously due to the sole choice that was available
    // therefore, need to skip this vertex and go on
    if (!_dlx.inDLX(TargetColumnCell)) {
        return X_star(bfsIndex+1, recordPartialResult, Depth);
    }


#ifdef DEBUG_MODE_EXACTSOLVER
    if (!PriorityColumnCell) cout << "bfsIndex : " << bfsIndex;
    else cout << "priority taken, ";
    cout << "  covering column " << *TargetColumnCell << endl;
#endif

    // get related rows
    Cell* tmp = TargetColumnCell->down;
    vector<Cell*> RelatedRow;
    while (tmp->Type() == NORMAL_CELL) { RelatedRow.push_back(tmp->left); tmp = tmp->down; }


#ifdef DEBUG_XSTAR
    // cout << "TargetColumnCell " << *TargetColumnCell << ", Depth " << Depth << endl;
#endif

    // conflict found!
    if (!RelatedRow.size()) {
#ifdef DEBUG_XSTAR
        // cout << "column uncoverable!" << endl;
#endif
        Vertex* ConflictVertex = TargetColumnCell->GetCorrespondVertex();

        if (recordPartialResult) {
            _solution[_component_id].push_back(new RowHeaderCell(ConflictVertex, UNDEF));
        }

        if (_graph.size(_component_id) == _solution[_component_id].size()) return CONFLICT_BUT_DONE;
        return CONFLICT_NOT_DONE;
    }
#ifdef DEBUG_XSTAR
    // else cout << "column coverable~~" << endl;
#endif

    // for each row in related rows
    stack<Cell*> AffectedCells;
    SolverState state = STATE_UNDEFINED;
    bool recoredResultNextLevel = false;
    for (unsigned int i = 0; i < RelatedRow.size(); ++i) {
#ifdef DEBUG_XSTAR
    // cout << '\r' << right << setw(6) << _dlx.cellcount() << " cells left in dlx, solving CC " << setw(4) << _component_id << '/' << _graph.numComponents()-1 << flush;
    // cout << right << setw(6) << _dlx.cellcount() << " cells left in dlx, solving CC " << setw(4) << _component_id << '/' << _graph.numComponents()-1 << " Depth " << Depth << " i: " << i << '/' << RelatedRow.size()-1 << " vertex num " << _graph.size(_component_id) << endl;
    // if (i == RelatedRow.size()-1)
        // cout << " Depth " << Depth << " i: " << i << '/' << RelatedRow.size()-1 << endl;
    // cout << endl << endl;
    // this->print();
#endif

        /***********************************************************************
        *  this controls whether to record the solution if a conflict is found *
        *  this is only set to 'true' if all of the for loops in the recursive *
        *  function is in the last loop                                        *
        *  i.e. X_star wiil terminate after the conflict is reported           *
        ***********************************************************************/
        if (i == RelatedRow.size() - 1) recoredResultNextLevel = true&recordPartialResult;
        else recoredResultNextLevel = false;

        _solution[_component_id].push_back(RelatedRow[i]);                  // pick this row

        CoverAffectedCells(RelatedRow[i], AffectedCells);    // cover the cells that is related to this row

        state = X_star(bfsIndex+1, recoredResultNextLevel, Depth+1);  // search for the next node by BFS order
        if (state == SUCCESS) break;                         // if a solution is found, stop searching

        UNCoverAffectedCells(AffectedCells);                 // conflict occurs, try another row that is related to this column

        if (!recoredResultNextLevel) _solution[_component_id].pop_back();   // remember to undo the changes that were made previously
    }

    assert(state != STATE_UNDEFINED);
    return state;
}

#endif

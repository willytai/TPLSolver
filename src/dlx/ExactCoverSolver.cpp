#include "ExactCoverSolver.h"
#include <iomanip>
#include <cassert>

void ExactCoverSolver::InitByFile(fstream& file) {
    _graph.ContstructByFile(file);
    InitByGraph(_graph);
    _solution.clear();
}

void ExactCoverSolver::InitByGraph(Graph& g) {
    _dlx.init(g);
}

void ExactCoverSolver::Solve() {
    SolverState result = X_star(0, true);

    while (result != SUCCESS) {
        cout << endl << "solution:" << endl;
        for (auto it = _solution.begin(); it != _solution.end(); ++it)
            cout << *(*it) << endl;
        cout << "Conflict found, need to remove and keep on searching" << endl;

        IdentifyUncolorablePartAndRemove();
        _solution.clear();
        result = X_star(0, true);
    }

    cout << endl << "solution:" << endl;
    for (auto it = _solution.begin(); it != _solution.end(); ++it)
        cout << *(*it) << endl;

    if (result == SUCCESS) cout << "Success!" << endl;
}

void ExactCoverSolver::report(ostream& os, string filename) {
    _graph.reportConflictSubgraphs(os, " Filename: "+filename);
}

void ExactCoverSolver::CoverAffectedCells(const Cell* refCell, stack<Cell*>& AffectedCells) {
    assert(refCell->Type() == ROW_HEADER_CELL);
    // cout << "ref: " << *refCell << endl << endl;
    // find affected columns
    // in this part, the picked row is not dealt with
    Cell* tmp = refCell->right;
    while (tmp->Type() == NORMAL_CELL) {
        Cell* columnElement = tmp->down;
        while (true) {
            if (columnElement->Type() != NORMAL_CELL) { columnElement = columnElement->down; continue; }
            if (tmp == columnElement) break;
            Cell* rowElement = columnElement;
            // cout << "removing entire row realted to " << *(_dlx.FindCorrespondRowHeader(rowElement)) << endl;
            while (true) {
                _dlx.remove(rowElement);
                AffectedCells.push(rowElement);
                // cout << *rowElement << endl;
                rowElement = rowElement->right;
                if (rowElement == rowElement->right) {
                    _dlx.remove(rowElement);
                    // cout << *rowElement << endl;
                    AffectedCells.push(rowElement);
                    break;
                }
            } // cout << endl;
            columnElement = columnElement->down;
        }
        tmp = tmp->right;
    }

	// deal with the picked row in this part
    // remove the picked row and the header
    tmp = refCell->right;
    // cout << "removing entire row realted to " << *(_dlx.FindCorrespondRowHeader(tmp)) << endl;
    while (true) {
        if (tmp->Type() == ROW_HEADER_CELL) {
            _dlx.remove(tmp);
            AffectedCells.push(tmp);
            // cout << *tmp << endl;
            break;
        }
        _dlx.remove(tmp);
        AffectedCells.push(tmp);
        // cout << *tmp << endl;
        Cell* columnHeader = _dlx.FindCorrespondColumnHeader(tmp);
        _dlx.remove(columnHeader);
        // cout << *columnHeader << endl;
        AffectedCells.push(columnHeader);
        tmp = tmp->right;
    }
    // assert(0);
}

void ExactCoverSolver::UNCoverAffectedCells(stack<Cell*>& AffectedCells) {
    while (!AffectedCells.empty()) {
        _dlx.recover(AffectedCells.top());
#ifdef DEBUG_MODE_EXACTSOLVER
        if (AffectedCells.top()->Type() == VERTEX_CELL) cout << "recovering vertex cell: " << *(AffectedCells.top()) << endl;
#endif
        AffectedCells.pop();
    }
}

Cell* ExactCoverSolver::FindPriorityColumn(const Cell* header) {
    Cell* tmp = header->right;
    while (tmp->Type() == VERTEX_CELL) {
        if (tmp->down->down == tmp) return tmp;
        tmp = tmp->right;
    }
    return NULL;
}

SolverState ExactCoverSolver::X_star(int bfsIndex, bool recordPartialResult) {
    if (_dlx.isGoal()) return SUCCESS;

    /*******************************/
    /* cover vertex with bfs order */
    /*******************************/
    // cover target column
    // check if there is any vertex that can be colored (only one color choice remains)
    // column with only one NormalCell
    Cell* PriorityColumnCell = FindPriorityColumn(_dlx.GetHeader());

    // if this happens, it means that after some edges removed by the identifier, some isolated vertex appears
    // therefore, there will be no need to consider the order of the vertex traversed after this point
    // just simply try to cover the column to the right of the _dlx header
    if (size_t(bfsIndex) > _graph.size()-1) {
        PriorityColumnCell = _dlx.GetHeader()->right;
#ifdef DEBUG_MODE_EXACTSOLVER
        cout << "Toggling isolated vertex, ID: " << PriorityColumnCell->GetCorrespondVertex()->ID << endl;
#endif
    }

    // otherwise, cover with bfs order
    Cell* TargetColumnCell = PriorityColumnCell;
    if (!PriorityColumnCell) {
        TargetColumnCell = _dlx.Column(_graph[bfsIndex]->ID);
    }
    // maintain bfs order
    else --bfsIndex;

    // if this is false, it means that this column is covered preivously due to the sole choice that was available
    // therefore, need to skip this vertex and go on
    if (!_dlx.inDLX(TargetColumnCell)) {
        return X_star(bfsIndex+1, recordPartialResult);
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

    // conflict found!
    if (!RelatedRow.size()) {
        Vertex* ConflictVertex = TargetColumnCell->GetCorrespondVertex();

        if (recordPartialResult) {
            _solution.push_back(new RowHeaderCell(ConflictVertex, UNDEF));
        }

        if (_graph.size() == _solution.size()) return CONFLICT_BUT_DONE;
        return CONFLICT_NOT_DONE;
    }

    // for each row in related rows
    stack<Cell*> AffectedCells;
    SolverState state = STATE_UNDEFINED;
    bool recoredResultNextLevel = false;
    for (unsigned int i = 0; i < RelatedRow.size(); ++i) {

        /***********************************************************************
        *  this controls whether to record the solution if a conflict is found *
        *  this is only set to 'true' if all of the for loops in the recursive *
        *  function is in the last loop                                        *
        *  i.e. X_star wiil terminate after the conflict is reported           *
        ***********************************************************************/
        if (i == RelatedRow.size() - 1) recoredResultNextLevel = true&recordPartialResult;
        else recoredResultNextLevel = false;

        _solution.push_back(RelatedRow[i]);                  // pick this row

        CoverAffectedCells(RelatedRow[i], AffectedCells);    // cover the cells that is related to this row

        state = X_star(bfsIndex+1, recoredResultNextLevel);  // search for the next node by BFS order
        if (state == SUCCESS) break;                         // if a solution is found, stop searching

        UNCoverAffectedCells(AffectedCells);                 // conflict occurs, try another row that is related to this column

        if (!recoredResultNextLevel) _solution.pop_back();   // remember to undo the changes that were made previously
    }

    assert(state != STATE_UNDEFINED);
    return state;
}

void ExactCoverSolver::IdentifyUncolorablePartAndRemove() {
    _graph.ApplySolution(_solution, _solution.back()->GetCorrespondVertex()->ID);
    _graph.runIdentification();
    vector<pair<int, int> > ConflictEdges;
    _graph.GetLatestConflictEdges(ConflictEdges);
    _dlx.removeConflictEdges(ConflictEdges);
}

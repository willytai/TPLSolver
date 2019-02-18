#include "ExactCoverSolver.h"
#include <iomanip>
#include <cassert>

void ExactCoverSolver::InitByFile(fstream& file) {
    cerr << "Method InitByFile(fstream& file) removed from ExactCoverSolver!" << endl;
    assert(false);
    _graph.ContstructByFile(file);
    //this->InitByGraph(_graph);
}

void ExactCoverSolver::InitByAdjList(fstream& file) {
    _graph.ContstructByAdjList(file);
}

void ExactCoverSolver::Solve() {
#ifndef ENABLE_PRIORITY_VERTEX
    cerr << "[Warning] Priority Column Disabled" << endl;
#endif
    _solution.resize(_graph.numComponents());
    for (int i = 0; i < _graph.numComponents(); ++i) {
#ifdef DEBUG_MODE_EDGES
        if (i != 2) continue;
#endif
        cout << "Solving Component " << i << ", " << _graph.size(i) << " vertices" << endl;
        _component_id = i;
        solve(i);
        // if (i == 2) break;
    }
}

void ExactCoverSolver::report(ostream& os, string filename, bool dump_adjlist) {
    _graph.reportConflictSubgraphs(os,   " Filename: "+filename); // write to file if filename specified
    _graph.reportConflictSubgraphs(cout, " Filename: "+filename); // display conflict part on screen
    for (int i = 0; i < _solution.size(); ++i) {
        _graph.ApplySolution(_solution[i], 1); // root id doesn't matter
    }
    // write to file if filename specified
    os << "-    Final Result    -" << endl;
    os << "----------------------" << endl;
    os << "conflict vertices: ";
    vector<int> conflict_vID;
    _graph.GetConflictID(conflict_vID);
    for (auto it = conflict_vID.begin(); it != conflict_vID.end(); ++it) {
        os << *it;
        if (it != --(conflict_vID.end())) os << ' ';
    } os << endl;
    for (int i = 0; i < _solution.size(); ++i) {
        for (auto it = _solution[i].begin(); it != _solution[i].end(); ++it) {
            const Vertex* v = ((*it)->GetCorrespondVertex());
            os << "vertexID," << setw(4) << v->ID << ",color," << v->color << endl;
        }
    }

    if (dump_adjlist) { 
        for (int i = 0; i < _solution.size(); ++i) {
            _graph.write_adjlist(os, _solution[i]);
        }
    }
}

void ExactCoverSolver::CoverAffectedCells(const Cell* refCell, stack<Cell*>& AffectedCells) {
    assert(refCell->Type() == ROW_HEADER_CELL);
    // find affected columns
    // in this part, the picked row is not dealt with
    Cell* tmp = refCell->right;
    while (tmp->Type() == NORMAL_CELL) {
        Cell* columnElement = tmp->down;
        while (true) {
            if (columnElement->Type() != NORMAL_CELL) { columnElement = columnElement->down; continue; }
            if (tmp == columnElement) break;
            Cell* rowElement = columnElement;
            while (true) {
                _dlx.remove(rowElement);
                AffectedCells.push(rowElement);
                rowElement = rowElement->right;
                if (rowElement == rowElement->right) {
                    _dlx.remove(rowElement);
                    AffectedCells.push(rowElement);
                    break;
                }
            }
            columnElement = columnElement->down;
        }
        tmp = tmp->right;
    }

	// deal with the picked row in this part
    // remove the picked row and the header
    tmp = refCell->right;
    while (true) {
        if (tmp->Type() == ROW_HEADER_CELL) {
            _dlx.remove(tmp);
            AffectedCells.push(tmp);
            break;
        }
        _dlx.remove(tmp);
        AffectedCells.push(tmp);
        Cell* columnHeader = _dlx.FindCorrespondColumnHeader(tmp);
        _dlx.remove(columnHeader);
        AffectedCells.push(columnHeader);
        tmp = tmp->right;
    }
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
        if (tmp->down == tmp) return tmp;         // detects conflict earlier
        tmp = tmp->right;
    }
    return NULL;
}

void ExactCoverSolver::IdentifyUncolorablePartAndRemove() {
    _graph.ApplySolution(_solution[_component_id], _solution[_component_id].back()->GetCorrespondVertex()->ID);
    cerr << "Identifing uncolorable part...";
    _graph.runIdentification(_component_id);
    cerr << "Done" << endl;
    vector<pair<int, int> > ConflictEdges;
    _graph.GetLatestConflictEdges(ConflictEdges);
    cerr << "Removing conflict edges...";
    _dlx.removeConflictEdges(ConflictEdges);
    cerr << "Done" << endl;
}

#ifdef DEBUG_XSTAR
void ExactCoverSolver::print() const {
    Cell* hh = _dlx.GetHeader();
    Cell* tmp = hh;
    while (true) {
        cout << *tmp << ' ';
        tmp = tmp->right;
        if (tmp->Type() == EDGE_CELL || tmp == hh) break;
    } cout << endl;
    tmp = hh->down;
    while (true) {
        Cell* haha = tmp;
        while (true) {
            cout << *haha << ' ';
            haha = haha->right;
            if (haha == tmp) break;
        } cout << endl;
        tmp = tmp->down;
        if (tmp == hh) break;
    } cout << endl;
    tmp = hh;
    while (true) {
        Cell* asdf = tmp;
        while (true) {
            cout << *asdf << ' ';
            asdf = asdf->down;
            if (asdf == tmp) break;
        } cout << endl;
        tmp = tmp->right;
        if (tmp->Type() == EDGE_CELL || tmp == hh) break;
    } cout << endl;
}
#endif

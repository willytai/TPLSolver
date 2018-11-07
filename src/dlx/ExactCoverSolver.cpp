#include "ExactCoverSolver.h"
#include <cassert>

void ExactCoverSolver::InitByFile(fstream& file) {
    _graph.ContstructByFile(file);
    InitByGraph(_graph);
}

void ExactCoverSolver::InitByGraph(Graph& g) {
    _dlx.init(g);
}

void ExactCoverSolver::Solve() {
    bool result = X_star(0);
    cout << "result: " << result << endl;
    for (auto it = _solution.begin(); it != _solution.end(); ++it)
        cout << *(*it) << endl;
}

bool ExactCoverSolver::X_star(int bfsIndex) {
    if (_dlx.isGoal()) return true;

    /*******************************/
    /* cover vertex with bfs order */
    /*******************************/
    // cover target column
    // check if there is any vertex that can be colored (only one color choice remains)
    // column with only one NormalCell
    Cell* PriorityColumnCell = FindPriorityColumn(_dlx.GetHeader());
    Cell* TargetColumnCell   = (PriorityColumnCell ? PriorityColumnCell : _dlx.Column(_graph[bfsIndex]->ID));

    // get related rows
    Cell* tmp = TargetColumnCell->down;
    vector<Cell*> RelatedRow;
    while (tmp->Type() == NORMAL_CELL) { RelatedRow.push_back(tmp->left); tmp = tmp->down; }

    if (!RelatedRow.size()) {
        Vertex* ConflictVertex = TargetColumnCell->GetCorrespondVertex();
        auto it = _solution.end(); --it;
        for (; it != _solution.begin(); --it) {
            const Vertex* target = (*it)->GetCorrespondVertex();
            auto itt = ConflictVertex->VertexList.begin();
            bool found = false;
            for (; itt != ConflictVertex->VertexList.end(); ++itt) {
                if (target == *itt) { found = true; break; }
            }
            if (found) break;
        }
        cout << *TargetColumnCell << " is uncolored, ";
        cout << "conflict with " << *(*it) << endl;
        cout << "current solution:" << endl;
        for (auto it = _solution.begin(); it != _solution.end(); ++it)
            cout << *(*it) << endl;
        return false;
    }

    // for each row in related rows
    stack<Cell*> AffectedCells;
    for (unsigned int i = 0; i < RelatedRow.size(); ++i) {
        _solution.push_back(RelatedRow[i]);
        CoverAffectedCells(RelatedRow[i], AffectedCells);
        if (X_star(bfsIndex+1)) return true;
        UNCoverAffectedCells(AffectedCells);
        _solution.pop_back();
    }

    return false;
}

void ExactCoverSolver::CoverAffectedCells(const Cell* refCell, stack<Cell*>& AffectedCells) {
    assert(refCell->Type() == ROW_HEADER_CELL);
    // cout << "ref: " << *refCell << endl << endl;
    // find affected columns
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

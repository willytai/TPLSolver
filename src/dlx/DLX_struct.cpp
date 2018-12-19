#include "DLX_struct.h"
#include <stack>
#include <cassert>
#include <iostream>

using namespace std;

bool DancingLink::isGoal() const {
    if (_header->right->Type() != VERTEX_CELL) return true;
    return false;
}

bool DancingLink::inDLX(Cell* c) const {
    return (c->left->right == c && c->right->left == c && c->up->down == c && c->down->up == c);
}

Cell* DancingLink::GetHeader() const {
    return _header;
}

void DancingLink::clear() {
    // in X_star, the result will return imediately if a solution is found
    // therefore, the dlx will be broken, making it unable to be tracked
    /* stack<Cell*> S;
    for (unsigned int i = 1; i < _rowHeader.size(); ++i) {
        Cell* tmp = _rowHeader[i];
        Cell* ref = tmp;
        while (tmp->right != ref) {
            S.push(tmp);
            tmp = tmp->right;
        }
    }
    while (!S.empty()) {
        delete S.top();
        S.pop();
    }*/
    for (unsigned int i = 0; i < _rowHeader.size(); ++i) {
        delete _rowHeader[i];
    }
    for (unsigned int i = 0; i < _columnHeader.size(); ++i) {
        if (!_columnHeader[i]) delete _columnHeader[i];
    }
    _columnHeader.clear();
    _rowHeader.clear();
#ifdef DEBUG_XSTAR
    _cellCount = 0;
#endif
}

void DancingLink::init(Graph& g, const int& component_id) {
#ifdef DEBUG_XSTAR
    _cellCount = 0;
#endif
    cerr << "Initializing dlx...";
    initHeader(g, component_id);
    initCell();
    cerr << "Done" << endl;
}

void DancingLink::initHeader(Graph& g, const int& component_id) {
    vector<Vertex*> vertexes;
    map<int, map<int, Edge*> > edges;
    g.RetrieveVertexes(vertexes, component_id);
    g.RetrieveEdges(edges, component_id);

#ifdef DEBUG_MODE_EDGES
    // check if edges are legal
    this->check_edge(edges);
#endif

    /* Contstruct the column header
     * The IDs of the vertexes would be the 
     * indexes of _columnHeader
     * i.e. _columnHeader[1] stores the vertex with ID = 1
     * _columnHeader[0] is reserved for the header of the whole
     * DancingLink structure
     */
    _columnHeader.resize(1, NULL);
    _columnHeader[0] = new NormalCell();
    int prev_id = 0;
    // for (unsigned int id = 1; id < vertexes.size(); ++id) {
    for (auto it = vertexes.begin(); it != vertexes.end(); ++it) {
        if (_columnHeader.size() < (*it)->ID+1) _columnHeader.resize((*it)->ID+1);
        _columnHeader[(*it)->ID] = new VertexCell(*it);
        Insert_Right(_columnHeader[(*it)->ID], _columnHeader[prev_id]);
        prev_id = (*it)->ID;
    }
    /*for (auto it = vertexes.begin(); it != vertexes.end(); ++it) {
        _columnHeader.push_back(new VertexCell(*it));
        Insert_Right(_columnHeader[_columnHeader.size()-1], _columnHeader[_columnHeader.size()-2]);
    }*/
    // for (unsigned int i = 0; i < edges.size(); ++i) {
    for (auto it = edges.begin(); it != edges.end(); ++it) {
        for (auto ti = (it->second).begin(); ti != (it->second).end(); ++ti) {
        // Cell* MainEdgeCell = new EdgeCell(edges[i], RED);
        Cell* MainEdgeCell = new EdgeCell(ti->second, RED);
        _columnHeader.push_back(MainEdgeCell);
        Insert_Right(_columnHeader[_columnHeader.size()-1], _columnHeader[prev_id]); prev_id = _columnHeader.size() - 1;
        // _columnHeader.push_back(new EdgeCell(edges[i], GREEN));
        _columnHeader.push_back(new EdgeCell(ti->second, GREEN));
        Insert_Right(_columnHeader[_columnHeader.size()-1], _columnHeader[prev_id]); prev_id = _columnHeader.size() - 1;
        // _columnHeader.push_back(new EdgeCell(edges[i], BLUE));
        _columnHeader.push_back(new EdgeCell(ti->second, BLUE));
        Insert_Right(_columnHeader[_columnHeader.size()-1], _columnHeader[prev_id]); prev_id = _columnHeader.size() - 1;

        // save the information to the corresponding VertexCell
        int vertex_id_1 = MainEdgeCell->GetCorrespondEdge()->v1_id;
        int vertex_id_2 = MainEdgeCell->GetCorrespondEdge()->v2_id;
        _columnHeader[vertex_id_1]->RecordEdgeCellPtr(MainEdgeCell);
        _columnHeader[vertex_id_2]->RecordEdgeCellPtr(MainEdgeCell);
        }
    }

    /* construct the row header
     * each vertexes will need three different rows
     * to represent three different color
     * let _rowHeader[0] be _header i.e. _columnHeader[0]
     * resize to number of vertexes + 1
     * the additional 1 is for the header of the whole dlx
     * the number of vertexes is actually vertexes.size()-1
     */
    _rowHeader.resize(3*(vertexes.size())+1, NULL);
    _rowHeader[0] = _columnHeader[0];
    for (unsigned int i = 0; i < vertexes.size(); ++i) {
       _rowHeader[3*i+1] = new RowHeaderCell(vertexes[i], RED);
       Insert_Down(_rowHeader[3*i+1], _rowHeader[3*i+0]);
       _rowHeader[3*i+2] = new RowHeaderCell(vertexes[i], GREEN);
       Insert_Down(_rowHeader[3*i+2], _rowHeader[3*i+1]);
       _rowHeader[3*i+3] = new RowHeaderCell(vertexes[i], BLUE);
       Insert_Down(_rowHeader[3*i+3], _rowHeader[3*i+2]);
    }

#ifdef DEBUG_MODE_DLX
    cout << endl << "Column Header" << endl;
    Cell* tmp = _header;
    while (true) {
        cout << *tmp << endl;
        tmp = tmp->right;
        if (tmp == _header) break;
    } cout << endl;
    cout << "Row Header" << endl;
    while (true) {
        cout << *tmp << endl;
        tmp = tmp->down;
        if (tmp == _header) break;
    } cout << endl;
#endif
}

void DancingLink::initCell() {
    for (unsigned int i = 1; i < _rowHeader.size(); ++i) {
        int   target_vertex_id = _rowHeader[i]->GetCorrespondVertex()->ID;
        Color target_color     = _rowHeader[i]->GetCellColor();

        Cell* VertexNode = new NormalCell();
#ifdef DEBUG_XSTAR
        ++_cellCount;
#endif
        Insert_Right(VertexNode, _rowHeader[i]);

        Cell* above = _columnHeader[target_vertex_id];
        while (above->down->Type() == NORMAL_CELL) { above = above->down; }
        Insert_Down(VertexNode, above);

        Cell* left = VertexNode;
        vector<Cell*> EdgeCells;
        _columnHeader[target_vertex_id]->GetEdgeCellPtr(EdgeCells);
        for (unsigned int i = 0; i < EdgeCells.size(); ++i) {
            Cell* EdgeNode = new NormalCell();
#ifdef DEBUG_XSTAR
            ++_cellCount;
#endif
            Insert_Right(EdgeNode, left);
            left = EdgeNode;

            above = EdgeCells[i];
            while (above->GetCellColor() != target_color) { above = above->right; }
            while (above->down->Type() == NORMAL_CELL) { above = above->down; }
            Insert_Down(EdgeNode, above);
        }
    }
#ifdef DEBUG_MODE_DLX
    cout << "Row Representation" << endl;
    for (unsigned int i = 1; i < _rowHeader.size(); ++i) {
        Cell* tmp = _rowHeader[i];
        while (tmp->right->Type() != ROW_HEADER_CELL) { cout << *tmp << " "; tmp = tmp->right; }
        cout << *tmp << endl;
    } cout << endl;
    cout << "Column representation" << endl;
    for (unsigned int i = 1; i < _columnHeader.size(); ++i) {
        Cell* tmp = _columnHeader[i];
        if (!tmp) continue;
        while (tmp->down->Type() == NORMAL_CELL) { cout << *tmp << " "; tmp = tmp->down; }
        cout << *tmp << endl;
    } cout << endl;
#endif
}

void DancingLink::removeConflictEdges(const vector<pair<int, int> >& Cedges) {
    for (auto it = Cedges.begin(); it != Cedges.end(); ++it) {
        const int& id_1 = (*it).first;
        vector<Cell*> EdgeCellCandidate; _columnHeader[id_1]->GetEdgeCellPtr(EdgeCellCandidate);
        for (auto iter = EdgeCellCandidate.begin(); iter != EdgeCellCandidate.end(); ++iter) {
            if ((*iter)->GetCorrespondEdge()->v2_id == (*it).second) {
                this->removeEntireColumn(*iter);
                this->removeEntireColumn((*iter)->right);
                this->removeEntireColumn((*iter)->right->right);
#ifdef DEBUG_MODE_DLX
                cout << "column " << **iter << " removed" << endl;
                cout << "column " << *((*iter)->right) << " removed" << endl;
                cout << "column " << *((*iter)->right->right) << " removed" << endl;
#endif
                break;
            }
        }
    }
}

Cell* DancingLink::Column(const int& idx) const {
    assert(idx);
    return _columnHeader[idx];
}

Cell* DancingLink::Row(const int& idx) const {
    assert(idx);
    return _rowHeader[idx];
}


/**********************************************************************
*                    functions for DLX operations                    *
**********************************************************************/

// set virtical links to itself
void DancingLink::UD_self(Cell* c) {
    c->up   = c;
    c->down = c;
}
// set horizontal links to itself
void DancingLink::LR_self(Cell* c) {
    c->left  = c;
    c->right = c;
}

// remove virtical links
void DancingLink::UD_remove(Cell* c) {
    c->up->down = c->down;
    c->down->up = c->up;
}

// remove horizontal links
void DancingLink::LR_remove(Cell* c) {
    c->left->right = c->right;
    c->right->left = c->left;
}

void DancingLink::remove(Cell* c) {
    UD_remove(c);
    LR_remove(c);
#ifdef DEBUG_XSTAR
    if (c->Type() == NORMAL_CELL) --_cellCount;
#endif
}

void DancingLink::recover(Cell* c) {
    UD_recover(c);
    LR_recover(c);
#ifdef DEBUG_XSTAR
    if (c->Type() == NORMAL_CELL) ++_cellCount;
#endif
}

// recover virtical links
void DancingLink::UD_recover(Cell* c) {
    c->up->down = c;
    c->down->up = c;
}

// recover horizontal links
void DancingLink::LR_recover(Cell* c) {
    c->left->right = c;
    c->right->left = c;
}


// insert 'c' to the right of 'ref'
void DancingLink::Insert_Right(Cell* c, Cell* ref) {
    c->left          = ref;
    c->right         = ref->right;
    ref->right->left = c;
    ref->right       = c;
}

// insert 'c' to the bottom of 'ref'
void DancingLink::Insert_Down(Cell* c, Cell* ref) {
    c->up         = ref;
    c->down       = ref->down;
    ref->down->up = c;
    ref->down     = c;
}

Cell* DancingLink::FindCorrespondColumnHeader(Cell*& c) {
    Cell* tmp = c;
    while (tmp->Type() == NORMAL_CELL) { tmp = tmp->up; }
    return tmp;
}

Cell* DancingLink::FindCorrespondRowHeader(Cell*& c) {
    Cell* tmp = c;
    while (tmp->Type() == NORMAL_CELL) { tmp = tmp->left; }
    return tmp;
}

void DancingLink::removeEntireColumn(Cell* ref) {
    while (ref->down != ref) {
        this->remove(ref);
        ref = ref->down;
    }
    this->remove(ref);
}

#ifdef DEBUG_MODE_EDGES
void DancingLink::check_edge(map<int, map<int, Edge*> >& edges) {
    for (auto it1 = edges.begin(); it1 != edges.end(); ++it1) {
        const auto& key1 = (*it1).first;
        const auto& value = (*it1).second;
        for (auto it2 = value.begin(); it2 != value.end(); ++it2) {
            const auto& key2 = (*it2).first;
            auto check1 = edges.find(key2);
            if (check1 == edges.end()) {
                cout << "edges error1, " << key1 << ", " << key2 << endl;
                assert(0);
            }
            auto check2 = (*check1).second.find(key1);
            if (check2 == (*check1).second.end()) {
                cout << "edges error2: " << key1 << ", " << key2 << endl;
                assert(0);
            }
        }
    }
    cout << "Edge test passed" << endl;
    exit(0);
}
#endif

#include "graph.h"
#include <sstream>
#include <iomanip>
#include <queue>

using namespace std;


/**********************************************************************
*                 the identification part in graph.h                  *
**********************************************************************/

void Graph::ApplySolution(const vector<Cell*>& sol, int RootID) {
    for (auto it = sol.begin(); it != sol.end(); ++it) {
        _vertex[(*it)->GetCorrespondVertex()->ID]->color = (*it)->GetCellColor();
    }
    _root = _vertex[RootID];
    assert(_root);
}

void Graph::runIdentification(const int& component_id) {
    /**************************************************
    * the color of the vertexes will all be restored  *
    * and the detected conflict edges will be removed *
    **************************************************/
    // reset all the vertexes to VERTEX_STATE_NOT_DETERMINED
    for (auto it = _temporary_colorable_vertexes.begin(); it != _temporary_colorable_vertexes.end(); ++it) {
        (*it)->state = VERTEX_STATE_NOT_DETERMINED;
#ifdef DEBUG_MODE_IDENT
        // cout << "reseting vertex " << *(*it) << " to state undetermined" << endl;
#endif
    }
    if (_conflict_subgraphs.size()) {
        for (auto it = _conflict_subgraphs.back().begin(); it != _conflict_subgraphs.back().end(); ++it) {
            (*it)->state = VERTEX_STATE_NOT_DETERMINED;
#ifdef DEBUG_MODE_IDENT
            // cout << "reseting vertex " << *(*it) << " to state undetermined" << endl;
#endif
        }
    }
    _temporary_colorable_vertexes.clear();

    Vertex::setGlobalRef();
    _conflict_subgraphs.resize(_conflict_subgraphs.size()+1); // the container for the next subgraph
    this->propagate(_root);
    this->RemoveEdges(component_id);

    cout << "New identified conflict vertexes" << endl;
    assert(_conflict_subgraphs.back().size());
    for (auto it = _conflict_subgraphs.back().begin(); it != _conflict_subgraphs.back().end(); ++it)
        cout << *(*it) << ' ';
    cout << endl;

    // because some edges will be removed, need to re-run bfs
    bfs(component_id, -1);
}

void Graph::reportConflictSubgraphs(ostream& os, string filename) const {
    os << "****************************************************" << endl;
    os << "*          Uncolorable Subgraph Reporter           *" << endl;
    os << "****************************************************" << endl;
    os << "|" << left << setw(50) << filename << "|" << endl;
    os << "----------------------------------------------------" << endl;
    os << "| There are " << setw(3) << _conflict_subgraphs.size() << "  uncolorable parts                 |" << endl;
    os << "----------------------------------------------------" << endl;
    for (unsigned int i = 0; i < _conflict_subgraphs.size(); ++i) {
        os << "| subgraph " << setw(40) << left << i+1 << "|" << endl;
        os << "| ";
        int edgecount = 0;
        stringstream ss;
        for (auto it = _conflict_subgraphs.at(i).begin(); it != _conflict_subgraphs.at(i).end(); ++it) {
            ss << "V " << setw(2) << *(*it) << ", ";
            if (++edgecount == 5) {
                edgecount = 0;
                os << left << setw(49) << ss.str() << "|" << endl << "| ";
                ss.str("");
            }
        }
        os << left << setw(49) << ss.str() << "|" << endl; 
        os << "----------------------------------------------------" << endl;
    }
}

void Graph::RemoveEdges(const int& component_id) {
    /****************************************************************************
    * find all the conflict edges (formed by two adjacent uncolorbale vertexes) *
    ****************************************************************************/
    Vertex::setGlobalRef();
    _latest_conflict_edges.clear();
    for (auto it = _conflict_subgraphs.back().begin(); it != _conflict_subgraphs.back().end(); ++it) {
        (*it)->setToGlobalRef();
        for (auto index_it = _adjList[(*it)->ID].begin(); index_it != _adjList[(*it)->ID].end(); ++index_it) {
            if (_vertex[*index_it]->isGlobalRef()) continue;
            if (_vertex[*index_it]->state != VERTEX_UNCOLORABLE) continue;

            // set the vertex ID in acending order
            if ((*it)->ID < *index_it) _latest_conflict_edges.push_back(pair<int, int>((*it)->ID, *index_it));
            else _latest_conflict_edges.push_back(pair<int, int>(*index_it, (*it)->ID));

        }
    }
    for (auto it = _latest_conflict_edges.begin(); it != _latest_conflict_edges.end(); ++it) {
        RemoveEdge((*it).first, (*it).second, component_id);
    }
}

void Graph::GetLatestConflictEdges(vector<pair<int, int> >& cedges) {
    cedges = _latest_conflict_edges;
}

bool Graph::propagate(Vertex* currentVertex) {
    if (currentVertex->state == VERTEX_COLORABLE)   return true;
    if (currentVertex->state == VERTEX_UNCOLORABLE) return false;

    /***********************************************
    * To check if the vertex is uncolorlabe        *
    * refer to check(Vertex*& currentVertex) below *
    ***********************************************/
    vector<bool> color_checker;
    if (!check(currentVertex, color_checker)) return false;

    const Color originalColor = currentVertex->color;    // original color, needs to restore it back to allow further traversal
    currentVertex->setToGlobalRef();                     // mark the visited vertex

    bool returnValue = true;                             // this will be set to false if any propagate called bellow returns false
    for (int i = 1; i <= 3; ++i) {
        if (!color_checker[i]) continue;
#ifdef DEBUG_MODE_IDENT
        // cout << "vertex: " << *currentVertex << ", color " << COLORS[i] << "; ";
#endif

        bool LegalColor = true;
        currentVertex->color = COLORS[i];                // set the color

#ifdef DEBUG_MODE_IDENT
        // check if after setting this color, any of the adjacent vertexes who were marked VERTEX_COLORABLE becomes uncolorable
        bool skip = false;
        for (auto it = _adjList[currentVertex->ID].begin(); it != _adjList[currentVertex->ID].end(); ++it) {
            if (_vertex[*it]->state != VERTEX_COLORABLE) continue;
            vector<bool> tmp;
            if (!check(_vertex[*it], tmp)) {
                // cout << "currentVertex: " << *currentVertex << ", made vertex " << *_vertex[*it];
                // cout << " who was marked colorable uncolorable after setting its color to " << COLORS[i] << endl;

                // don't know if this is correct //
                _vertex[*it]->state = VERTEX_UNCOLORABLE;
                // don't know if this is correct //

                skip = true;
                break;
            }
        }
        if (skip) continue;
#endif

        for (auto it = _adjList[currentVertex->ID].begin(); it != _adjList[currentVertex->ID].end(); ++it) {

            // skip the non-conflict vertexes, vertexes that does not contribute to the conflict will be skipped as well (their colors would all be UNDEF)
            if (_vertex[*it]->color != currentVertex->color) continue;

            // currentVertex would not be colorable with COLORS[i]
            LegalColor = false;

#ifdef DEBUG_MODE_IDENT
            // cout << "currentVertex: " << *currentVertex << ", trying color " << COLORS[i] << "; ";
            // cout << "found conflict with vertex: " << *_vertex[*it] << ", propagate" << endl;
#endif

            if (!propagate(_vertex[*it])) { // mark edge (currentVertex, *it) as a conflict edge
#ifdef DEBUG_MODE
                // cout << "(" << *currentVertex << ", " << *_vertex[*it] << ") is a conflict edge" << endl;
#endif
                // sort the vertex id in ascending order
                // if (currentVertex->ID > *it) _conflict_subgraphs.back().push_back(pair<int, int>(*it, currentVertex->ID));
                // else _conflict_subgraphs.back().push_back(pair<int, int>(currentVertex->ID, *it));
                returnValue = false;                     // returns false if at least the value of one branch is false
            }
        }
        if (LegalColor) { // if a color is found to be legal, simply return true
            returnValue = true;
            break;
        }
    }
#ifdef DEBUG_MODE_IDENT
    // cout << "currentVertex: " << *currentVertex << ", coloring it back to " << originalColor << endl;
    // cout << "returning " << returnValue << " at vertex: " << *currentVertex << endl;
#endif
    currentVertex->color = originalColor;                          // restore the color back
    currentVertex->unSetGlobalRef();                               // unmark the vertex
    if (!returnValue) {
        currentVertex->state = VERTEX_UNCOLORABLE;   // set the state of the vertex
        _conflict_subgraphs.back().push_back(currentVertex);
    }
    else {
        _temporary_colorable_vertexes.push_back(currentVertex);
        currentVertex->state = VERTEX_COLORABLE;
    }
    return returnValue;
}

inline bool Graph::check(Vertex*& currentVertex, vector<bool>& color_checker) {
    bool colorable  = false;
    color_checker.resize(5, true);   // index 0 is dummy node, index 4 is for UNDEF, only 1~3 is actually used

    // To check if the vertex is colorable
    for (auto it = _adjList[currentVertex->ID].begin(); it != _adjList[currentVertex->ID].end(); ++it) {
        if (_vertex[*it]->color == UNDEF) continue;   // skip the vertexes that do not contribute to the conflict
        if (_vertex[*it]->isGlobalRef()) {
            color_checker[int(_vertex[*it]->color)] = false;  // mark the used color
        }
    }
    for (size_t i = 1; i <= 3; ++i) {
        if (color_checker[i]) {
            colorable = true;
            break;
        }
    }

    if (!colorable) {
        /******************************************************************************
        * if a vertex is found to be uncolorable, do not keep searching, return false *
        * this avoids going into a infinite loop                                      *
        ******************************************************************************/
        currentVertex->state = VERTEX_UNCOLORABLE;
        _conflict_subgraphs.back().push_back(currentVertex);
        return false;
    }
    return true;
}

inline bool Graph::not_colorable_due_to_ident_process(int& id) {
    return not_colorable_due_to_ident_process(_vertex[id]);
}

inline bool Graph::not_colorable_due_to_ident_process(Vertex*& v) {
    bool color_checker[] = {true, true, true, true};
    for (auto it = _adjList[v->ID].begin(); it != _adjList[v->ID].end(); ++it) {
        if (_vertex[*it]->color == UNDEF) continue;
        if (!_vertex[*it]->isGlobalRef()) continue;
        color_checker[int(_vertex[*it]->color)] = false;
        if (color_checker[1] == false && color_checker[2] == false && color_checker[3] == false) {
            if (v->state == VERTEX_COLORABLE) return true;
        }
    }
    return false;
}

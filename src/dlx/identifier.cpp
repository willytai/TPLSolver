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
    assert(!_vertex[0]);
    assert(_root);
}

void Graph::runIdentification() {
    /**************************************************
    * the color of the vertexes will all be restored  *
    * and the detected conflict edges will be removed *
    **************************************************/
    Vertex::setGlobalRef();
    _conflict_subgraphs.resize(_conflict_subgraphs.size()+1);
    propagate(_root);
    for (auto it = _conflict_subgraphs.back().begin(); it != _conflict_subgraphs.back().end(); ++it) {
        RemoveEdge(it->first, it->second);
    }
    // because some edges will be removed, need to re-run bfs
    bfs();
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
            ss << "(" << setw(2) << (*it).first << ", " << setw(2) << (*it).second << ") ";
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

void Graph::GetLatestConflictEdges(vector<pair<int, int> >& cedges) {
    cedges = _conflict_subgraphs.back();
}

bool Graph::propagate(Vertex* currentVertex) {
    if (currentVertex->state == VERTEX_COLORABLE)   return true;
    if (currentVertex->state == VERTEX_UNCOLORABLE) return false;

    /***********************************************
    * To check if the vertex is uncolorlabe        *
    * refer to check(Vertex*& currentVertex) below *
    ***********************************************/
    if (!check(currentVertex)) return false;

    const Color originalColor = currentVertex->color;    // original color, needs to restore it back to allow further traversal
    currentVertex->setToGlobalRef();                     // mark the visited vertex

    bool returnValue = true;                             // this will be set to false if any propagate called bellow returns false
    for (int i = 1; i <= 3; ++i) {
        if (COLORS[i] == originalColor) continue;        // avoid its parent's color, i.e. don't search backwards

        //cout << "vertex: " << *currentVertex << ", color " << COLORS[i] << "; ";

        bool LegalColor = true;
        currentVertex->color = COLORS[i];                // set the color

        for (auto it = _adjList[currentVertex->ID].begin(); it != _adjList[currentVertex->ID].end(); ++it) {

            // skip the non-conflict vertexes, vertexes that does not contribute to the conflict will be skipped as well (their colors would all be UNDEF)
            if (_vertex[*it]->color != currentVertex->color) continue;
            
            // currentVertex would not be colorable with COLORS[i]
            LegalColor = false;

            if (_vertex[*it]->isGlobalRef()) {
                //cout << "vertex: " << *(*it) << " is globalref; ";
                /****************************************************************************************
                * ignore the visited vertex whose color is COLORS[i]                                    *
                * if 'break' instead of 'continue' is used, some conflict edges will not be collected   *
                * fail_case_2 is an example                                                             *
                * the color of vertex 3 and 6 would be COLORS[i], but only 3 is visited                 *
                * normally we try a different color and see if the pattern works, but edge (current, 6) *
                * needs to be collected first                                                           *
                ****************************************************************************************/
                continue;
            }

            //cout << "currentVertex: " << *currentVertex << ", trying color " << COLORS[i] << "; ";
            //cout << "found conflict with vertex: " << *_vertex[*it] << ", propagate" << endl;

            if (!propagate(_vertex[*it])) { // mark edge (currentVertex, *it) as a conflict edge
#ifdef DEBUG_MODE
                cout << "(" << *currentVertex << ", " << *_vertex[*it] << ") is a conflict edge" << endl;
#endif
                // sort the vertex id in ascending order
                if (currentVertex->ID > *it) _conflict_subgraphs.back().push_back(pair<int, int>(*it, currentVertex->ID));
                else _conflict_subgraphs.back().push_back(pair<int, int>(currentVertex->ID, *it));
                returnValue = false;                     // returns false if at least the value of one branch is false
            }
        }
        if (LegalColor) { // if a color is found to be legal, simply return true
            returnValue = true;
            break;
        }
    }
    //cout << "currentVertex: " << *currentVertex << ", coloring it back to " << originalColor << endl;
    //cout << "returning " << returnValue << " at vertex: " << *currentVertex << endl;
    currentVertex->color = originalColor;                          // restore the color back
    currentVertex->unSetGlobalRef();                               // unmark the vertex
    if (!returnValue) currentVertex->state = VERTEX_UNCOLORABLE;   // set the state of the vertex
    else currentVertex->state = VERTEX_COLORABLE;
    return returnValue;
}

inline bool Graph::check(Vertex*& currentVertex) {
    bool colorable  = false;
    vector<bool> color_checker; color_checker.resize(5, true);   // index 0 is dummy node, index 4 is for UNDEF, only 1~3 is actually used

    // To check if the vertex is colorable
    for (auto it = _adjList[currentVertex->ID].begin(); it != _adjList[currentVertex->ID].end(); ++it) {
        if (_vertex[*it]->color == UNDEF) continue;                                        // skip the vertexes that do not contribute to the conflict
        if (_vertex[*it]->isGlobalRef()) color_checker[int(_vertex[*it]->color)] = false;  // mark the used color
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
        return false;
    }
    return true;
}

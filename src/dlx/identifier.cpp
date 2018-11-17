#include "graph.h"
#include <queue>

using namespace std;


/**********************************************************************
*                 the identification part in graph.h                 *
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
    _conflict_edges.clear();
    Vertex::setGlobalRef();
    propagate(_root);
    for (auto it = _conflict_edges.begin(); it != _conflict_edges.end(); ++it) {
        RemoveEdge(it->first, it->second);
    }
}

void Graph::GetConflictEdges(vector<pair<int, int> >& cedges) {
    cedges = _conflict_edges;
}

bool Graph::propagate(Vertex* currentVertex) {
    if (currentVertex->state == VERTEX_COLORABLE) return true;
    if (currentVertex->state == VERTEX_UNCOLORABLE) return false;

    /***********************************************
    * refer to check(Vertex*& currentVertex) below *
    ***********************************************/
    CheckState checkstate = check(currentVertex);
    if (checkstate == RETURN_FALSE) return false;
    else if (checkstate == RETURN_TRUE) return true;

    const Color originalColor = currentVertex->color;    // original color, needs to restore it back to allow further traversal
    currentVertex->setToGlobalRef();                     // mark the visited vertex

    bool returnValue = true;                             // this will be set to false if any propagate called bellow returns false
    for (int i = 1; i <= 3; ++i) {
        if (COLORS[i] == originalColor) continue;        // avoid its parent's color, i.e. don't search backwards

        //cout << "vertex: " << *currentVertex << ", color " << COLORS[i] << "; ";

        currentVertex->color = COLORS[i];                // set the color

        // for (auto it = currentVertex->VertexList.begin(); it != currentVertex->VertexList.end(); ++it) {
        for (auto it = _adjList[currentVertex->ID].begin(); it != _adjList[currentVertex->ID].end(); ++it) {

            // skip the non-conflict vertexes, vertexes that does not contribute to the conflict will be skipped as well (their colors would all be UNDEF)
            if (_vertex[*it]->color != currentVertex->color) continue;

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
            //cout << "found conflict with vertex: " << *(*it) << ", propagate" << endl;

            if (!propagate(_vertex[*it])) {              // mark edge (currentVertex, *it) as a conflict edge
                cout << "(" << *currentVertex << ", " << *_vertex[*it] << ") is a conflict edge" << endl;

                // sort the vertex id in ascending order
                if (currentVertex->ID > *it) _conflict_edges.push_back(pair<int, int>(*it, currentVertex->ID));
                else _conflict_edges.push_back(pair<int, int>(currentVertex->ID, *it));
                returnValue = false;                     // returns false if at least the value of one branch is false
            }
        }
    }
    currentVertex->color = originalColor;                // restore the color back
    currentVertex->unSetGlobalRef();                     // unmark the vertex
    //cout << "currentVertex: " << *currentVertex << ", coloring it back to " << originalColor << endl;
    //cout << "returning " << returnValue << " at vertex: " << *currentVertex << endl;
    if (!returnValue) currentVertex->state = VERTEX_UNCOLORABLE;   // set the state of the vertex
    else currentVertex->state = VERTEX_COLORABLE;
    return returnValue;
}

inline CheckState Graph::check(Vertex*& currentVertex) {
    bool* dummy = new bool(false);
    bool determined = true;
    bool colorable  = false;
    CheckState checkstate = KEEP_SEARCHING;                      // default CheckState
    vector<bool*> color_checker; color_checker.resize(5, dummy); // index 0 is dummy node, index 4 is for UNDEF, only 1~3 is actually used

    // for (auto it = currentVertex->VertexList.begin(); it != currentVertex->VertexList.end(); ++it) {
    for (auto it = _adjList[currentVertex->ID].begin(); it != _adjList[currentVertex->ID].end(); ++it) {
        if (!_vertex[*it]) continue;                                 // skip the vertexes that do not contribute to the conflict
        if (!_vertex[*it]->isGlobalRef()) { determined = false; }    // there are still some other vertexes left
        else color_checker[int(_vertex[*it]->color)] = NULL;         // mark the used color
    }
    for (size_t i = 1; i <= 3; ++i) {
        if (color_checker[i]) colorable = true;
    }
    if (!colorable) {
        /******************************************************************************
        * if a vertex is found to be uncolorable, do not keep searching, return false *
        * this avoids going into a infinite loop                                      *
        ******************************************************************************/
        currentVertex->state = VERTEX_UNCOLORABLE;
        checkstate = RETURN_FALSE;
    }
    else if (colorable && determined) {
        /************************************************************************************
        * if found vertex colorable and is determined, return true                          *
        * this avoids returning true too early before the child vertexes are not even tried *
        ************************************************************************************/
        currentVertex->state = VERTEX_COLORABLE;
        checkstate = RETURN_TRUE;
    }

    delete dummy;  // free up memory
    return checkstate;
}

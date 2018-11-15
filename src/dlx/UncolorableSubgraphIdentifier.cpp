#include "UncolorableSubgraphIdentifier.h"
#include <queue>

using namespace std;


void UncolorableSubgraphIdentifier::InitByColoredVertexes(const vector<Cell*>& sol, int RootID, int VertexNum) {
    _vertex.resize(VertexNum+1, NULL);
    for (auto it = sol.begin(); it != sol.end(); ++it) {
        Vertex* currentVertex     = (*it)->GetCorrespondVertex();
        const Color& currentColor = (*it)->GetCellColor();
        _vertex[currentVertex->ID] = currentVertex;
        currentVertex->color = currentColor;
    }
    _root = _vertex[RootID];
    assert(!_vertex[0]);
    assert(_root);
}

void UncolorableSubgraphIdentifier::run() {
    Vertex::setGlobalRef();
    propagate(_root);
}

bool UncolorableSubgraphIdentifier::propagate(Vertex* currentVertex) {
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

        for (auto it = currentVertex->VertexList.begin(); it != currentVertex->VertexList.end(); ++it) {

            // skip the non-conflict vertexes, vertexes that does not contribute to the conflict will be skipped as well (their colors would all be UNDEF)
            if ((*it)->color != currentVertex->color) continue;     

            if ((*it)->isGlobalRef()) {
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

            if (!propagate(*it)) {                       // mark edge (currentVertex, *it) as a conflict edge
                cout << "(" << *currentVertex << ", " << *(*it) << ") is a conflict edge" << endl;
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

inline CheckState UncolorableSubgraphIdentifier::check(Vertex*& currentVertex) {
    bool* dummy = new bool(false);
    bool determined = true;
    bool colorable  = false;
    CheckState checkstate = KEEP_SEARCHING;                      // default CheckState
    vector<bool*> color_checker; color_checker.resize(5, dummy); // index 0 is dummy node, index 4 is for UNDEF, only 1~3 is actually used

    for (auto it = currentVertex->VertexList.begin(); it != currentVertex->VertexList.end(); ++it) {
        if (!_vertex[(*it)->ID]) continue;                       // skip the vertexes that do not contribute to the conflict
        if (!(*it)->isGlobalRef()) { determined = false; }       // there are still some other vertexes left
        else color_checker[int((*it)->color)] = NULL;            // mark the used color
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

void UncolorableSubgraphIdentifier::bfs() {
    Vertex::setGlobalRef();
    queue<Vertex*> Q;
    Q.push(_root);
    while (!Q.empty()) {
        Vertex* current = Q.front();
        Q.pop();
        if (current->isGlobalRef()) continue;
        for (auto it = current->VertexList.begin(); it != current->VertexList.end(); ++it) {
            if (!_vertex[(*it)->ID]) continue;
            if ((*it)->isGlobalRef()) continue;
            Q.push(*it);
        }
        current->setToGlobalRef();
        cout << "vertex ID:" << *current << " with color " << current->color << endl;
    }
}

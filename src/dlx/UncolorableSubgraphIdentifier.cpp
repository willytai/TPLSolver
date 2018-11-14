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
    // if (currentVertex->isGlobalRef()) return false;      // a dead end occurs
    // if no other vertexes are left to try, and currentVertex is colorable, then return true
    bool* dummy = new bool(false);
    vector<bool*> color_checker; color_checker.resize(5, dummy); // index 0 is dummy node, index 4 is for UNDEF, only 1~3 is actually used
    bool determined = true;
    bool colorable  = false;
    for (auto it = currentVertex->VertexList.begin(); it != currentVertex->VertexList.end(); ++it) {
        if (!_vertex[(*it)->ID]) continue;
        if (!(*it)->isGlobalRef()) { determined = false; } // there are still some other vertexes left
        else color_checker[int((*it)->color)] = NULL;
    }
    for (size_t i = 1; i <= 3; ++i) {
        if (color_checker[i] && determined) { cout << "vertex " << *currentVertex << " is colorable" << endl; return true; } // colorable
        if (color_checker[i]) colorable = true;
    }
    if (!colorable) return false;                        // if found vertex uncolorable, just return

    const Color originalColor = currentVertex->color;    // original color, need to restore it back to allow further traversal
    currentVertex->setToGlobalRef();                     // mark the visited vertex

    bool returnValue = true;                             // this will be set to false if any propagate called bellow returns false
    for (int i = 1; i <= 3; ++i) {
        if (COLORS[i] == originalColor) continue;        // avoid its parent's color


        currentVertex->color = COLORS[i];
        for (auto it = currentVertex->VertexList.begin(); it != currentVertex->VertexList.end(); ++it) {
            if ((*it)->color != currentVertex->color) continue;     // skip the non-conflict vertexes
            if ((*it)->isGlobalRef()) break;                        // if a vertex is already visited and forms a conflict, then this color is not available
                                                                    // needs to try a different one

            cout << "currentVertex: " << *currentVertex << ", trying color " << COLORS[i] << "; ";
            cout << "found conflict with vertex: " << *(*it) << ", propagate" << endl;

            if (!propagate(*it)) {                       // mark edge (currentVertex, *it) as a conflict edge
                cout << "(" << *currentVertex << ", " << *(*it) << ") is a conflict edge" << endl;
                returnValue = false;                     // returns false if at least the value of one branch is false
            }
            // else returnValue |= true;
        }
    }
    currentVertex->color = originalColor;                // restore the color
    currentVertex->unSetGlobalRef();                     // unmark the vertex
    cout << "currentVertex: " << *currentVertex << ", coloring it back to " << originalColor << endl;
    cout << "returning " << returnValue << " at vertex: " << *currentVertex << endl;
    delete dummy;  // free up memory
    return returnValue;
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

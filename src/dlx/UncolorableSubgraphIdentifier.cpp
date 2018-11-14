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
    if (currentVertex->isGlobalRef()) return false;      // a dead end occurs

    const Color originalColor = currentVertex->color;    // original color, need to restore it back to allow further traversal
    currentVertex->setToGlobalRef();                     // mark the visited vertex

    bool returnValue = true;
    for (int i = 1; i <= 3; ++i) {
        if (COLORS[i] == originalColor) continue;        // avoid its parent's color

        cout << "currentVertex: " << *currentVertex << ", trying color " << COLORS[i] << "; ";

        currentVertex->color = COLORS[i];
        for (auto it = currentVertex->VertexList.begin(); it != currentVertex->VertexList.end(); ++it) {
            if ((*it)->color != currentVertex->color) continue;     // skip the non-conflict vertexes

            cout << "found conflict with vertex: " << *(*it) << ", propagate" << endl;
            
            if (!propagate(*it)) {                       // mark edge (currentVertex, *it) as a conflict edge
                cout << "(" << *currentVertex << ", " << *(*it) << ") is a conflict edge" << endl;
                returnValue = false;                     // returns false if at least the value of one branch is false
            }
        }
    }
    currentVertex->color = originalColor;                // restore the color
    cout << "currentVertex: " << *currentVertex << ", coloring it back to " << originalColor << endl;
    cout << "returning " << returnValue << " at vertex: " << *currentVertex << endl;
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

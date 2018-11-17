#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include "color.h"

using namespace std;

// 16 byte
struct Vertex
{
    Vertex(int id) : color(UNDEF), state(VERTEX_STATE_NOT_DETERMINED) { ID = id; }
    
    int                 ID;
    Color               color;
    VertexState         state; // consumes memory, very costly

    // overload cout
    friend ostream& operator << (ostream& os, const Vertex& v) {
        os << v.ID;
        return os;
    }

    // for graph traversal
    static unsigned _globalRef;
    unsigned _ref;
    static void setGlobalRef () { ++_globalRef; }
    void setToGlobalRef      () { _ref = _globalRef; }
    void unSetGlobalRef      () { assert(isGlobalRef()); _ref--; }
    bool isGlobalRef         () { return _ref == _globalRef; }
};

#endif /* VERTEX_H */

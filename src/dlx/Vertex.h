#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <vector>
#include "color.h"

using namespace std;

struct Vertex
{
    Vertex(int id) : color(UNDEF) { ID = id; }
    
    Color color;
    int ID;
    vector<Vertex*> VertexList;

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

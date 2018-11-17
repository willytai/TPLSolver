#ifndef CELL_H
#define CELL_H

#include <vector>
#include <iostream>
#include "color.h"
#include "Vertex.h"
#include "Edge.h"

using namespace std;

enum CellType
{
    ROW_HEADER_CELL,
    VERTEX_CELL,
    EDGE_CELL,
    NORMAL_CELL
};

struct Cell
{
    Cell () { left = right = up = down = this; }
    virtual ~Cell() {}
    
    virtual CellType Type                 ()               const = 0;               // returns cell type
    virtual Color    GetCellColor         ()               const { return UNDEF; }  // returns the color of the cell if defined
    virtual Edge*    GetCorrespondEdge    ()               const { return NULL; }   // returns the edge to the corresponding edge column
    virtual Vertex*  GetCorrespondVertex  ()               const { return NULL; }   // returns the vertex to the corresponding vertex column
    virtual void     RecordEdgeCellPtr    (Cell*)                { return; }        // to speed up the construction of the DLX
    virtual void     GetEdgeCellPtr       (vector<Cell*>&) const { return; }        // to speed up the construction of the DLX
    virtual void     print                (ostream& os)    const = 0;               // visualization

    friend ostream& operator << (ostream& os, const Cell& c) {
        c.print(os);
        return os;
    }

    Cell* left;
    Cell* right;
    Cell* up;
    Cell* down;
};

struct RowHeaderCell : Cell
{
    RowHeaderCell(Vertex* v, Color c = UNDEF) { vertex = v; color = c; }
    
    Color     GetCellColor        ()            const { return color; }
    Vertex*   GetCorrespondVertex ()            const { return vertex; }
    CellType  Type                ()            const { return ROW_HEADER_CELL; }
    void      print               (ostream& os) const {
        os << "<RowHeaderCell, VertexID:" << *vertex << ", color " << color << ">";
    }
    
    Vertex* vertex;
    Color   color;
};

struct VertexCell : Cell
{
    VertexCell(Vertex* v, Color c = UNDEF) { vertex = v; color = c; }
    
    Color    GetCellColor        ()                    const { return color; }
    Vertex*  GetCorrespondVertex ()                    const { return vertex; }
    CellType Type                ()                    const { return VERTEX_CELL; }
    void     RecordEdgeCellPtr   (Cell* c)                   { edge_cell_ptr.push_back(c); }
    void     GetEdgeCellPtr      (vector<Cell*>& ptrs) const { ptrs = edge_cell_ptr; }
    void     print               (ostream& os)         const {
        os << "<VertexCell, VertexID:" << *vertex << ", color " << color << ">";
    }

    Vertex* vertex;
    Color   color;

    vector<Cell*>   edge_cell_ptr;
};

struct EdgeCell: Cell
{
    EdgeCell(Edge* e, Color c = UNDEF) { edge = e; color = c; }

    Color    GetCellColor      ()            const { return color; }
    Edge*    GetCorrespondEdge ()            const { return edge; }
    CellType Type              ()            const { return EDGE_CELL; }
    void     print             (ostream& os) const {
        os << "<EdgeCell, " << *edge << ", color " << color << ">";
    }

    Edge*   edge;
    Color   color;
};

struct NormalCell : Cell
{
    // just an empty shell
    
    CellType Type ()             const { return NORMAL_CELL; }
    void     print (ostream& os) const { os << "<NormalCell>"; }
};

#endif /* CELL_H */

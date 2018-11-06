#ifndef DLX_STRUCT_H
#define DLX_STRUCT_H

#include<iostream>
#include "graph.h"
#include "color.h"

using namespace std;

struct Cell
{
    Cell () { left = right = up = down = this; }
    virtual ~Cell() {}
    
    virtual string  Type                 ()               const = 0;
    virtual Color   GetCellColor         ()               const { return UNDEF; }
    virtual Edge*   GetCorrespondEdge    ()               const { return NULL; }
    virtual Vertex* GetCorrespondVertex  ()               const { return NULL; }
    virtual void    RecordEdgeCellPtr    (Cell*)                { return; }
    virtual void    GetEdgeCellPtr       (vector<Cell*>&) const { return; }
    virtual void    print (ostream& os)                   const = 0;

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
    
    Color   GetCellColor        ()            const { return color; }
    Vertex* GetCorrespondVertex ()            const { return vertex; }
    void    print               (ostream& os) const {
        os << "<RowHeaderCell, VertexID:" << *vertex << ", color " << color << ">";
    }
    string  Type                ()            const { return "RowHeaderCell"; }
    
    Color   color;
    Vertex* vertex;
};

struct VertexCell : Cell
{
    VertexCell(Vertex* v, Color c = UNDEF) { vertex = v; color = c; }
    
    Color  GetCellColor      ()                    const { return color; }
    void   print             (ostream& os)         const {
        os << "<VertexCell, VertexID:" << *vertex << ", color " << color << ">";
    }
    void   RecordEdgeCellPtr (Cell* c)                   { edge_cell_ptr.push_back(c); }
    void   GetEdgeCellPtr    (vector<Cell*>& ptrs) const { ptrs = edge_cell_ptr; }
    string Type              ()                    const { return "VertexCell"; }
    
    Color   color;
    Vertex* vertex;

    vector<Cell*>   edge_cell_ptr;
};

struct EdgeCell: Cell
{
    EdgeCell(Edge* e, Color c = UNDEF) { edge = e; color = c; }

    Color  GetCellColor      ()            const { return color; }
    Edge*  GetCorrespondEdge ()            const { return edge; }
    void   print             (ostream& os) const {
        os << "<EdgeCell, " << *edge << ", color " << color << ">";
    }
    string Type              ()            const { return "EdgeCell"; }

    Color   color;
    Edge*   edge;
};

struct NormalCell : Cell
{
    // just an empty shell
    
    void   print (ostream& os) const { os << "<NormalCell>"; }
    string Type  ()            const { return "NormalCell"; }
};

class DancingLink
{
#define _header _columnHeader[0]
public:
    DancingLink() {}
    DancingLink(Graph& g) { init(g); }
    ~DancingLink() {}

    void  init       (Graph& g);
    void  initHeader (Graph& g);
    void  initCell   ();
    bool  isGoal     () const;
    Cell* GetHeader  () const;

    Cell* Column (const int& idx) const;
    Cell* Row    (const int& idx) const;


    /**********************************************************************
    *                    functions for DLX operations                    *
    **********************************************************************/

    // set virtical links to itself
    void UD_self(Cell* c);

    // set horizontal links to itself
    void LR_self(Cell* c);

    // remove virtical links
    void UD_remove(Cell* c);

    // remove horizontal links
    void LR_remove(Cell* c);

    void remove(Cell* c);

    // recover virtical links
    void UD_recover(Cell* c);

    // recover horizontal links
    void LR_recover(Cell* c);

    void recover(Cell* c);

    // insert 'c' to the right of 'ref'
    void Insert_Right(Cell* c, Cell* ref);

    // insert 'c' to the bottom of 'ref'
    void Insert_Down(Cell* c, Cell* ref);

    // find the corresponding column header
    Cell* FindCorrespondColumnHeader(Cell*& c);

    // find the corresponding row header
    Cell* FindCorrespondRowHeader(Cell*& c);


private:
    vector<Cell*>   _columnHeader;
    vector<Cell*>   _rowHeader;
};


#endif /* DLX_STRUCT_H */

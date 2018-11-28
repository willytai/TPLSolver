#ifndef DLX_STRUCT_H
#define DLX_STRUCT_H

#include <iostream>
#include "graph.h"
#include "Cell.h"

using namespace std;

class DancingLink
{
#define _header _columnHeader[0]
public:
    DancingLink() {}
    DancingLink(Graph& g) { init(g); }
    ~DancingLink() {}

    void  init                (Graph& g);
    void  initHeader          (Graph& g);
    void  initCell            ();
    void  removeConflictEdges (const vector<pair<int, int> >& Cedges);
    bool  isGoal              () const;
    bool  inDLX               (Cell* c) const;
    Cell* GetHeader           () const;

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

    // remove the entire cell
    void remove(Cell* c);

    // recover virtical links
    void UD_recover(Cell* c);

    // recover horizontal links
    void LR_recover(Cell* c);

    // recover entirely
    void recover(Cell* c);

    // insert 'c' to the right of 'ref'
    void Insert_Right(Cell* c, Cell* ref);

    // insert 'c' to the bottom of 'ref'
    void Insert_Down(Cell* c, Cell* ref);

    // find the corresponding column header
    Cell* FindCorrespondColumnHeader(Cell*& c);

    // find the corresponding row header
    Cell* FindCorrespondRowHeader(Cell*& c);

    // removes the entire column whose column header is ref
    void removeEntireColumn(Cell* ref);


private:
    vector<Cell*>   _columnHeader;
    vector<Cell*>   _rowHeader;
};


#endif /* DLX_STRUCT_H */

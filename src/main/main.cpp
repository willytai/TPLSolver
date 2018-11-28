#include <fstream>
#include <iostream>
#include "ExactCoverSolver.h"
#include "myUsage.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "usage: ./DLX <graph file> <report filename (default cout)>" << endl;
        return 0;
    }

    ofstream ofile;
    if (argc == 3) ofile.open(argv[2]);

    MyUsage usg;
    fstream file;
    file.open(argv[1]);

    if (!file.is_open()) {
        cerr << "failed to open " << argv[1] << endl;
        return 0;
    }

    ExactCoverSolver* solver = new ExactCoverSolver;
    usg.reset();
    solver->InitByFile(file);
    solver->Solve();
    if (!ofile.is_open()) 
        solver->report(cout, argv[1]);
    else
        solver->report(ofile, argv[1]);
    usg.report(1, 1);

#ifdef DEBUG_MODE
    cout << "size of Color:         " << sizeof(Color) << endl;
    cout << "size of Vertex:        " << sizeof(Vertex)<< endl;
    cout << "size of Edge:          " << sizeof(Edge)  << endl;
    cout << "size of NormalCell:    " << sizeof(NormalCell) << endl;
    cout << "size of VertexCell:    " << sizeof(VertexCell) << endl;
    cout << "size of RowHeaderCell: " << sizeof(RowHeaderCell) << endl;
    cout << "size of EdgeCell:      " << sizeof(EdgeCell) << endl;
#endif 

    delete solver;
    return 0;

    /*Graph g;
    g.ContstructByFile(file);
    g.print();
    g.RemoveVertex(1);
    g.RemoveVertex(2);
    g.RemoveVertex(3);
    g.RemoveVertex(4);
    g.RemoveVertex(5);
    g.RemoveVertex(6);
    g.RemoveVertex(7);
    g.RemoveVertex(8);
    g.bfs();
    g.print();
    g.RecoverVertex(1);
    g.bfs();
    g.print();

    return 0;*/
}

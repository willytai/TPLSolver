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
    solver->InitByAdjList(file);
    solver->Solve();
    ostream NullStream(NULL); // nothing will be printed if this is used as an ostream obj
    if (!ofile.is_open()) 
        solver->report(NullStream, argv[1]);
    else
        solver->report(ofile, argv[1]);
    usg.report(1, 1);

    delete solver;
    return 0;

/*
size of Color:         4
size of Vertex:        16
size of Edge:          8
size of NormalCell:    40
size of VertexCell:    80
size of RowHeaderCell: 56
size of EdgeCell:      56
*/
}

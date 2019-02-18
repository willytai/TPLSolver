#include <fstream>
#include <iostream>
#include "ExactCoverSolver.h"
#include "myUsage.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "usage: ./TPLSolver <adjacency list> <report filename (default cout)> <whether to dump adjacency list> (default 0)" << endl;
        return 0;
    }

    ofstream ofile;
    bool dump_adjlist = false;
    if (argc == 3) ofile.open(argv[2]);
    else if (argc == 4) dump_adjlist = true;

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
        solver->report(NullStream, argv[1], dump_adjlist);
    else
        solver->report(ofile, argv[1], dump_adjlist);
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

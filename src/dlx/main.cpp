#include <vector>
#include <fstream>
#include <iostream>
#include "ExactCoverSolver.h"

using namespace std;

int main(int argc, char *argv[])
{
    fstream file;
    file.open(argv[1]);

    if (!file.is_open()) {
        cerr << "failed to open " << argv[1] << endl;
        return 0;
    }

    ExactCoverSolver solver;
    solver.InitByFile(file);
    solver.Solve();
    return 0;

    Graph g;
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

    return 0;
}

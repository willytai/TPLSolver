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
}

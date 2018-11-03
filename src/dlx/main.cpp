#include <vector>
#include <fstream>
#include <iostream>
#include "graph.h"
#include "DLX_struct.h"

using namespace std;

int main(int argc, char *argv[])
{
    fstream file;
    file.open(argv[1]);

    if (!file.is_open()) {
        cerr << "failed to open " << argv[1] << endl;
        return 0;
    }

    Graph graph;
    graph.ContstructByFile(file);

    DancingLink dlx(graph);

    return 0;
}

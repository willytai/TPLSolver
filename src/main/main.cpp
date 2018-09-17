#include <iostream>
#include "TPLSolver.h"

using namespace std;


int main(int argc, char *argv[])
{
	cout << endl;

#ifdef DEBUG
	vector<Polygon*> debug;
#endif

	TPLSolver* solver = new TPLSolver;

#ifdef DEBUG
	solver->run(argv[1], debug);
#else
	solver->run(argv[1]);
#endif

#ifdef DEBUG
	cout << "- size of BBox:       " << sizeof(BBox) << endl;
	cout << "- size of Coordinate: " << sizeof(Coordinate) << endl;
	cout << "- size of Rectangle:  " << sizeof(Rectangle) << endl;
	cout << "- size of Polygon:    " << sizeof(Polygon) << endl;
	cout << "- size of parser:     " << sizeof(LayoutParser) << endl;
	cout << "- " << debug.size() << " polygons parsed" << endl;
#endif
	
	cout << endl;
	return 0;
}

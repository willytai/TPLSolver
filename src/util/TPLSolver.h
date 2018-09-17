#ifndef TPLSOLVER_H
#define TPLSOLVER_H

#include "LayoutParser.h"
#include "Shapes.h"
#include <string>
#include <vector>

using namespace std;

class TPLSolver
{
public:
    TPLSolver() {}
    ~TPLSolver() {}

#ifdef DEBUG
	void run(string s, vector<Polygon*>& d) {
		_parser.parse(s, d);
	}
#else
	void run(string s) {
		_parser.parse(s);
	}
#endif

private:
	// LayoutParser has no private members
	// store it as object will only take 1 byte
    LayoutParser _parser;
};

#endif /* TPLSOLVER_H */

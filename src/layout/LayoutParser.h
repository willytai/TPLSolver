#ifndef LAYOUTPARSER_H
#define LAYOUTPARSER_H

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Shapes.h"

using namespace std;

class LayoutParser{
public:
    LayoutParser() {}
    ~LayoutParser() {}

#ifdef DEBUG
	void parse(const string&, vector<Polygon*>&);
#else
	void parse(const string&);
#endif
private:
};

#endif /* LAYOUTPARSER_H */

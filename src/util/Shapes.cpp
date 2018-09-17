#include "Shapes.h"

bool Polygon::add_coor(Coordinate & c) {
	_coor.push_back(c);
	
	// needs at least four coordinates
	if (_coor.size() < 4) return false;

	// check whether coordinates form a closed shape
	if (_coor.front() == _coor.back()) return true;
	return false;
}

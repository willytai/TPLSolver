#include "Coordinate.h"
#include <iomanip>
#include <iostream>

using namespace std;

bool Coordinate::operator == (const Coordinate& co) const {
	return ( co.get_x() == _x && co.get_y() == _y );
}

void Coordinate::print() const {
	cout << '(' << setw(5) << _x << ", ";
	cout << setw(5) << _y << ')' << endl;
}

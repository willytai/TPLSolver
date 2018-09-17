#ifndef SHAPES_H
#define SHAPES_H

#include "Coordinate.h"
#include <vector>

using namespace std;

struct BBox
{
	BBox() {}
	~BBox() {}

	// access functions
	inline Coordinate bottom_left() const { return _bottom_left; }
	inline Coordinate upper_right() const { return _upper_right; }

	Coordinate _bottom_left;
	Coordinate _upper_right;
};


class Rectangle{
public:
    Rectangle() {}
    Rectangle(BBox b) { _bbox = b; }
    ~Rectangle() {}

	// access functions
    inline BBox bbox() const { return _bbox; }

private:
    BBox _bbox;
};

class Polygon{
public:
    Polygon() {}
    ~Polygon() {}

	// setup functions
    bool add_coor(Coordinate&);

private:
    vector<Coordinate> _coor;
};

#endif /* SHAPES_H */

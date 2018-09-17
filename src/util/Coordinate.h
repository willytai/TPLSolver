#ifndef COORDINATE_H
#define COORDINATE_H

class Coordinate{
public:
    Coordinate() { _x = _y = 0; }
    Coordinate(int x, int y) { _x = x; _y = y; }
    ~Coordinate() {}

	// access functions
	inline int  get_x() const { return _x; }
	inline int  get_y() const { return _y; }
	
	// verbosity
	void print() const;
	
	// overloading
	bool operator == (const Coordinate& co) const;

private:
    int _x;
    int _y;
};

#endif /* COORDINATE_H */

#include "LayoutParser.h"
#include "Coordinate.h"
#include "Shapes.h"
#include <iostream>
#include <cassert>
#include <climits>

using namespace std;

#ifdef DEBUG
// parser for debug mode
void LayoutParser::parse(const string& filename, vector<Polygon*>& debug) {
	// format printing
	stringstream ss, SS;
	ss << "= Layout file: " << filename << " =";
	for (unsigned int i = 0; i < ss.str().length()-2; ++i) SS << "=";
	cerr << SS.str() << endl;
	cerr << ss.str() << endl;
	cerr << SS.str() << endl;

	// open file
	fstream file;
	file.open(filename.c_str());
	if (!file.is_open()) {
		cerr << "Cannot open file: " << filename << endl;
	}

	// start parsing
	string buffer;
	while(getline(file, buffer)) {
		// coordinates
		if (buffer.substr(0, 2) != "XY") continue;

		// remove "XY: "
		istringstream iss(buffer.substr(4));

		// parse xy coordinates of polygon one by one
		string tmp;
		bool valid_polygon = false;   // check if polygon is valid
		int x = INT_MAX, y = INT_MAX; // buffer for the coordinates
		Polygon* polygon = new Polygon;
		while (iss >> tmp) {
			// start recording coordinates
			if (tmp.back() == ',') tmp.pop_back();

			// store x, y Coordinates temoprarily
			if (x == INT_MAX) x = stoi(tmp);
			else y = stoi(tmp);

			// got x and y
			if (y != INT_MAX) {
				Coordinate co(x, y);
				// co.print();
				valid_polygon = polygon->add_coor(co);
				x = y = INT_MAX;
			}
		}
		assert (valid_polygon); // check if polygon is valid
		debug.push_back(polygon);
	}
}
#else
void LayoutParser::parse(const string& filename) {
	// format printing
	stringstream ss, SS;
	ss << "= Layout file: " << filename << " =";
	for (unsigned int i = 0; i < ss.str().length()-2; ++i) SS << "=";
	cerr << SS.str() << endl;
	cerr << ss.str() << endl;
	cerr << SS.str() << endl;

	// open file
	fstream file;
	file.open(filename.c_str());
	if (!file.is_open()) {
		cerr << "Cannot open file: " << filename << endl;
	}

	// start parsing
	string buffer;
	while(getline(file, buffer)) {
		// coordinates
		if (buffer.substr(0, 2) != "XY") continue;

		// remove "XY: "
		istringstream iss(buffer.substr(4));

		// parse xy coordinates of polygon one by one
		string tmp;
		bool valid_polygon = false;   // check if polygon is valid
		int x = INT_MAX, y = INT_MAX; // buffer for the coordinates
		Polygon* polygon = new Polygon;
		while (iss >> tmp) {
			// start recording coordinates
			if (tmp.back() == ',') tmp.pop_back();

			// store x, y Coordinates temoprarily
			if (x == INT_MAX) x = stoi(tmp);
			else y = stoi(tmp);

			// got x and y
			if (y != INT_MAX) {
				Coordinate co(x, y);
				// co.print();
				valid_polygon = polygon->add_coor(co);
				x = y = INT_MAX;
			}
		}
		assert (valid_polygon); // check of polygon is valid
	}
}
#endif

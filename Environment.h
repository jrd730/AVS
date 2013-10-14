#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "QuadTree.h"
#include "Line.h"

using namespace std;

class Environment
{
	public:
		Environment ();
		~Environment ();

		void destroy ();

		void insertLineSegment (vertex v);
		bool insertEvenSpacedLine (vertex v, double spacing);
		void endLineSegment ();

		void printLineSegments (ostream& os);
		void drawLineSegments ();
		void drawLineQuadTree ();

		void insertPolygon (vertex v, int radius, int sides);
		
		QuadTree <Line*>* lineMap;

	private:

		vector <Line*> lines;
		vector <Line*> lineStarts;

		Line* curLine;
};
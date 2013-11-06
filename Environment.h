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
		void clear ();

		void insertLineSegment (vertex v);
		bool insertEvenSpacedLine (vertex v, double spacing);
		void endLineSegment ();

		void printLineSegments (ostream& os);
		void drawLineSegments ();
		void drawLineQuadTree ();

		void insertPolygon (vertex v, int radius, int sides);

		void insertWaypoint (vertex v);
		void drawWaypoints ();
		void drawWaypoint (const vertex& wp);
		
		QuadTree <Line*>* lineMap;

		vector <Line*> lines;
		vector <vertex> waypoints;

	private:

		vector <Line*> lineStarts;

		Line* curLine;
};
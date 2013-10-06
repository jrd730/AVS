#include <cstdlib>
#include <vector>

#include "QuadTree.h"
#include "Line.h"

using namespace std;

class Environment
{
	public:
		Environment ();
		~Environment ();

		void insertLineSegment (vertex v);
		void endLineSegment ();
		void drawLineSegments ();

		void insertPolygon (vertex v, int radius, int sides);

	private:

		vector <Line*> lines;
		Line* curLine;
};
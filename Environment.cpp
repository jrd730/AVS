#include "Environment.h"

static vertex origin (0, 0);
static vertex axis (128.0, 128.0);

Environment::Environment ()
{
	curLine = NULL;
	lineMap = new QuadTree <Line*> (origin, axis, 1, 26);
}

Environment::~Environment ()
{
	destroy();
}

void Environment::destroy ()
{
	delete lineMap;
	lineMap = new QuadTree <Line*> (origin, axis, 1, 26);

	for (int i=0; i < lines.size(); ++i){
		if (lines[i])
			delete lines[i];
	}

	lines.clear();
	lineStarts.clear();

	curLine = NULL;
}

void Environment::clear ()
{
	delete lineMap;
	lineMap = new QuadTree <Line*> (origin, axis, 1, 26);
	lines.clear ();
	lineStarts.clear();

	curLine = NULL;
}

void Environment::insertLineSegment (vertex v)
{
	if (!curLine){
		curLine = new Line (v);
		lines.push_back (curLine);
		lineMap->insert (v, curLine);
		lineStarts.push_back (curLine);
	}
	else{
		curLine = new Line (v, lines[lines.size()-1], NULL);
		lines[lines.size()-1]->setNextLine (curLine);
		lines.push_back (curLine);
		lineMap->insert (v, curLine);
	}
}

bool Environment::insertEvenSpacedLine (vertex v, double spacing)
{
	if (curLine){
		float dist = curLine->start.distance (v);
		if (dist >= spacing){
			// get vector from start to end points
			vertex vec ((v.x-curLine->start.x), (v.y-curLine->start.y));
			
			// unitize vector and make its distance equal to min line spacing
			vec = (vec*spacing)/dist;

			// create in between points to fill in the gap
			vertex tween (curLine->start.x+vec.x, curLine->start.y+vec.y);
			float coveredDistance = 0;
			do{
				coveredDistance += spacing;
				if (coveredDistance < dist){
					insertLineSegment (tween);
					tween = vertex (tween.x+vec.x, tween.y+vec.y);
				}
			}while (coveredDistance < dist);
			
			insertLineSegment (v);
		}
		else{
			return false;
		}
	}
	else{
		curLine = new Line (v);
		lines.push_back (curLine);
		lineMap->insert (v, curLine);
		lineStarts.push_back (curLine);
	}
	return true;
}

void Environment::endLineSegment ()
{
	curLine = NULL;
}

void Environment::printLineSegments (ostream& os)
{
	for (int i = 0; i < lineStarts.size(); ++i)
	{
		Line* l = lineStarts[i];
		os << "START_LINE\n";
		do{
			os << l->start.x << " " << l->start.y << endl;
			l = l->next;
		}while (l != NULL);
		os << "END_LINE\n";
	}
}

void Environment::drawLineSegments ()
{
	glColor3f (1, 1, 1);
	glBegin (GL_LINES);
	for (int i=0; i < lines.size(); ++i){
		if (lines[i]->next != NULL){
		glVertex2f (lines[i]->start.x, lines[i]->start.y);
		glVertex2f (lines[i]->next->start.x, lines[i]->next->start.y);}
	}
	glEnd();
}

void Environment::drawLineQuadTree ()
{
	if (lineMap){
		lineMap->draw();
	}
}

void Environment::insertPolygon (vertex v, int radius, int sides)
{

}
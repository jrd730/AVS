#include "Environment.h"

static vertex origin (0, 0);
static vertex axis (128.0, 128.0);

Environment::Environment ()
{
	curLine = NULL;
	lineMap = new QuadTree <Line*> (origin, axis, 1, 16);
}

Environment::~Environment ()
{
	destroy();
}

void Environment::destroy ()
{
	delete lineMap;
	lineMap = new QuadTree <Line*> (origin, axis, 1, 16);

	for (int i=0; i < lines.size(); ++i){
		delete lines[i];
	}

	lines.clear();
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
		if (curLine->start.distance (v) >= spacing){
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
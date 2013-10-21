/*
Lines are painted white lines on the obstacle course. The IGV is not allowed to pass through them.
*/
#pragma once
#include <cstdlib>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include "Vertex.h"

class Line
{
public:	
	Line (vertex v);
	Line (vertex v, Line* p, Line* n);
	~Line ();

	void setNextLine (Line* l);
	const Line* getNextLine ();

	void draw ();

	vertex start;
	Line* prev;
	Line* next;

private:
	
};
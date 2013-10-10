#include "Line.h"
Line::Line (vertex v){
	start = v;
	next = NULL;
	prev = NULL;
}

Line::Line(vertex v, Line* p, Line* n){
	start = v;
	next = n;
	prev = p;
}

Line::~Line (){}
void Line::setNextLine (Line* l){
	next = l;
}

const Line* Line::getNextLine (){
	return next;
}
/*
An intelligent ground vehicle is like a regular vehicle but with a brain and the ability to move about autonomously. 
*/
#pragma once
#include "DDV.h"
#include "Environment.h"
#include "NavMesh.h"
#include "QuadTree.h"

class IGV : public DDV
{
public:
	IGV ();
	~IGV ();

	void runProgram ();

  void draw ();

private:

	NavMesh paths;
	//QuadTree <int>* knownObstacles;
  Environment  env;

	double cameraMaxRange;
  double cameraMinRange;
	double cameraSpread;

  vertex vertices[6];
};
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

  void setSensorVertices ();

	NavMesh paths;
	//QuadTree <int>* knownObstacles;
  Environment  env;

	double cameraMaxRange; // meters from center
  double cameraMinRange; // meters from center
	double cameraSpread; // degrees

  vector <vertex> modelVertices; //meters from center
  vector <vertex> sensorVertices; //meters from center

};
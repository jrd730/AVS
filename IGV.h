/*
An intelligent ground vehicle is like a regular vehicle but with a brain and the ability to move about autonomously. 
*/
#pragma once
#include "DDV.h"
#include "Environment.h"
#include "NavMesh.h"
#include "Pathfinder.h"
#include "QuadTree.h"


class IGV : public DDV
{
public:

	IGV ();
	
  ~IGV ();

  void setCameraRange (double range);
  double getCameraRange ();

  void setCameraSpread (double spread);
  double getCameraSpread ();

  void setVisibleLines (vector <Line*>& lines);

  void addVisibleLinesToMap ();

  void findPath (vertex target);

	void runProgram ();

  void draw ();
	
  void setSensorVertices ();
  
  double cameraMaxRange; // meters from center
  double cameraMinRange; // meters from center
  double cameraSpread; // degrees
  
  vector <Line*> visibleLines;

  Environment  env;
  Pathfinder pf;

  NavMesh paths;
  //QuadTree <int>* knownObstacles;

  vector <vertex> modelVertices; //meters from center
  vector <vertex> sensorVertices; //meters from center

  bool autonomousMode; 
  bool followingPath;

  private:

    float angleTo (vertex v);

};
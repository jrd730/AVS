/*
An intelligent ground vehicle is like a regular vehicle but with a brain and the ability to move about autonomously. 
*/
#pragma once
#include <algorithm>
#include "DDV.h"
#include "Environment.h"
#include "Pathfinder.h"
#include "QuadTree.h"

struct region
{
  vertex minXY;
  vertex maxXY;
}; 

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
  void clearPath ();

	void runProgram ();

  void draw ();
	
  void displayInfo ();

  void setSensorVertices ();
  
  double cameraMaxRange;
  double cameraMinRange; 
  double cameraSpread; 
  
  vector <Line*> visibleLines;

  Environment  env;
  Pathfinder pf;

  vector <vertex> modelVertices; 
  vector <vertex> sensorVertices; 

  bool autonomousMode; 
  bool followingPath;

  float angleTo (vertex v);
  
  private:  
    region explored;
    vertex pathNode;

};
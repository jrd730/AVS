#include "IGV.h"

IGV::IGV ()
{
  modelVertices.resize (6);
  modelVertices[0] = vertex (-.325, .325);
  modelVertices[1] = vertex ( .325, .325);
  modelVertices[2] = vertex ( .325, -.325);
  modelVertices[3] = vertex ( .175, -.925);
  modelVertices[4] = vertex ( -.175, -.925);
  modelVertices[5] = vertex ( -.325, -.325);

  cameraMinRange = 0.1;
  cameraMaxRange = 3.0;
  cameraSpread = 70.0;
}

IGV::~IGV ()
{
	
}

void IGV::setSensorVertices ()
{
  sensorVertices.resize (3);
  
}

void IGV::runProgram ()
{
  
}

void IGV::draw ()
{
  // draw outline of robot chassis
  glColor3f (1.0, 1.0, 1.0);
  glBegin (GL_POLYGON);
  for (unsigned i = 0; i < modelVertices.size(); ++i){
    glVertex2f (modelVertices[i].x, modelVertices[i].y);
  }
  glEnd();

  // draw camera range
  glColor3f (0, 1.0, 0);
  glBegin (GL_LINE_LOOP);
  for (unsigned i = 0; i < modelVertices.size(); ++i){
    glVertex2f (modelVertices[i].x, modelVertices[i].y);
  }
  glEnd();

}
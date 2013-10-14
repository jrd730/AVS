#include "IGV.h"

static double base_speed = .01;

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
  cameraMaxRange = 5.0;
  cameraSpread = 70.0;

  setSensorVertices ();
}

IGV::~IGV ()
{
	
}

void IGV::setCameraRange (double range)
{
  cameraMaxRange = range;
  setSensorVertices ();
}

double IGV::getCameraRange ()
{
  return cameraMaxRange; 
}

void IGV::setCameraSpread (double spread)
{
  cameraSpread = spread;
  setSensorVertices ();
}

double IGV::getCameraSpread ()
{
  return cameraSpread; 
}

void IGV::setVisibleLines (vector <pair <vertex, Line*> >& lines)
{
  visibleLines.clear ();
  visibleLines.resize(lines.size());

  for (unsigned i = 0; i < lines.size(); ++i){
    visibleLines[i] = lines[i].second;
  }
}

void IGV::setSensorVertices ()
{
  sensorVertices.clear ();
  sensorVertices.resize (4);

  sensorVertices[0] = vertex (0, 0);
  sensorVertices[1] = 
  vertex ( cameraMaxRange * sin ((cameraSpread/DEGREES_PER_RADIAN)/2.0), 
            cameraMaxRange * cos ((cameraSpread/DEGREES_PER_RADIAN)/2.0) );
  
  sensorVertices[2] = 
  vertex ( cameraMaxRange * -sin ((cameraSpread/DEGREES_PER_RADIAN)/2.0), 
            cameraMaxRange * cos ((cameraSpread/DEGREES_PER_RADIAN)/2.0) );

  // sensorVertices[1] = vertex (-1, 3);
  // sensorVertices[2] = vertex (1, 3);
  // sensorVertices[3] = vertex (0, 0);

}

void IGV::runProgram ()
{
  
}

void IGV::draw ()
{
  glPushMatrix ();

  glTranslatef (position.x, position.y, 0);
  glRotatef (rotation, 0, 0, 1);

  // draw outline of robot chassis
  glColor3f (.5, 1.0, 1.0);
  glBegin (GL_POLYGON);
  for (unsigned i = 0; i < modelVertices.size(); ++i){
    glVertex2f (modelVertices[i].x, modelVertices[i].y);
  }
  glEnd();

  // draw camera range
  glColor3f (0, 1.0, 0);
  glBegin (GL_LINE_STRIP);
  for (unsigned i = 0; i < modelVertices.size(); ++i){
    glVertex2f (sensorVertices[i].x, sensorVertices[i].y);
  }
  glEnd();

  glPopMatrix ();

  glColor3f (0, 1.0, 0);
  glBegin (GL_LINES);
  for (unsigned i = 0; i < visibleLines.size(); ++i){
    glVertex2f (visibleLines[i]->start.x, visibleLines[i]->start.y);
    glVertex2f (visibleLines[i]->next->start.x, visibleLines[i]->next->start.y);
  }
  glEnd();

}
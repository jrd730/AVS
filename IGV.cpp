#include "IGV.h"

static double base_speed = .01;

IGV::IGV () :
  cameraMinRange (0.1),
  cameraMaxRange (5.0),
  cameraSpread   (70.0),
  autonomousMode (false),
  followingPath (false)
{
  modelVertices.resize (6);
  modelVertices[0] = vertex (-.325, .325);
  modelVertices[1] = vertex ( .325, .325);
  modelVertices[2] = vertex ( .325, -.325);
  modelVertices[3] = vertex ( .175, -.925);
  modelVertices[4] = vertex ( -.175, -.925);
  modelVertices[5] = vertex ( -.325, -.325);

  pf.set (8, 0.3, 0.2, 0.5);

  setSensorVertices ();
}

IGV::~IGV ()
{
	
}

void IGV::findPath (vertex target)
{
  pf.begin (env.lineMap, position, target);
  //while (pf.expand () && !pf.done ());
}

void IGV::runProgram ()
{
  addVisibleLinesToMap ();

  if (autonomousMode)
  {
    // still have goals to reach
    if (env.waypoints.size () > 0){
      
      // a route to a goal has been mapped out already
      if (followingPath)
      {
        // align to next node in the path

        // go forwards

        // check if current node is close enough to move on to next
      } 
      
      // generate a path to a goal
      else
      {
        // start a new path 
        if ( !pf.searching () ){
          findPath (env.waypoints[0]);
        }
        else{
          if ( !pf.done () ){
            pf.expand ();
          }
          else{
            followingPath = true;
          }
        }
      }
    }
  }
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

void IGV::setVisibleLines (vector <Line*>& lines)
{
  visibleLines.clear ();
  visibleLines.resize(lines.size());

  for (unsigned i = 0; i < lines.size(); ++i){
    visibleLines[i] = lines[i];
  }
}

void IGV::addVisibleLinesToMap ()
{
  for (int i = 0; i < visibleLines.size(); ++i){
    if (env.lineMap->insert (visibleLines[i]->start, visibleLines[i])){
      env.lines.push_back (visibleLines[i]);
    }
  }
}

void IGV::setSensorVertices ()
{
  sensorVertices.clear ();
  sensorVertices.resize (3);

  sensorVertices[0] = vertex (0, 0);
  sensorVertices[1] = 
  vertex ( cameraMaxRange * sin ((cameraSpread/DEGREES_PER_RADIAN)/2.0), 
            cameraMaxRange * cos ((cameraSpread/DEGREES_PER_RADIAN)/2.0) );
  
  sensorVertices[2] = 
  vertex ( cameraMaxRange * -sin ((cameraSpread/DEGREES_PER_RADIAN)/2.0), 
            cameraMaxRange * cos ((cameraSpread/DEGREES_PER_RADIAN)/2.0) );
}

float IGV::angleTo (vertex v)
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
  glBegin (GL_LINE_LOOP);
  for (unsigned i = 0; i < sensorVertices.size(); ++i){
    glVertex2f (sensorVertices[i].x, sensorVertices[i].y);
  }
  glEnd();

  glPopMatrix ();

  // glColor3f (0, 1.0, 0);
  // glBegin (GL_LINES);
  // for (unsigned i = 0; i < visibleLines.size(); ++i){
  //   glVertex2f (visibleLines[i]->start.x, visibleLines[i]->start.y);
  //   glVertex2f (visibleLines[i]->next->start.x, visibleLines[i]->next->start.y);
  // }
  // glEnd();

}
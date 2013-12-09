#include "Simulator.h"

static int height = 640;
static int width = 640;
static double ZOOM_INC = 128.0;
static double PAN_INC = 128.0;

static float graphXMin = -16;
static float graphXMax = 16;
static float graphXRange = graphXMax - graphXMin;
static float pixToXCoord = graphXRange/width;

static float graphYMin = -16;
static float graphYMax = 16;
static float graphYRange = graphYMax - graphYMin;
static float pixToYCoord = graphYRange/height;

static bool leftMouseDown = 0;
static bool rightMouseDown = 0;

static vertex origin (0, 0);
static vertex axis (128.0, 128.0);

static vector <vertex> targetPoint;
static vector <vertex> foundPoint;
static vector <Line*> visibleLines;
static vector <vertex> visiblePoints;

static int bucketSize = 1;
static bool drawQuadTree = 0;

static double minLineGap = 0.1;

static int gl_window_id;
static int gui_window_id;
static float glui_radius = 0;
static float glui_wall_distance = 0;
static float glui_crowd_distance = 0;
static float glui_goal_distance = 0;
static int glui_divisions = 0;

bool keyMask [255];
bool arrowKeys [4];

int drawIGV = 1;
int drawEnv = 1;
int drawSelectionBox = 0;
int drawVisibleLines = 1;

int displayMode = 0;
int heuristicMode = 0;

GLUI *glui;

static vertex pix_coord_to_point (int x, int y) { 
  return vertex ( x*pixToXCoord + graphXMin, -y*pixToYCoord + graphYMax);
}

Simulator* Simulator::callbackInstance (NULL);

static void initializeViewMatrix ()
{
    graphXRange = graphXMax - graphXMin;
    graphYRange = graphYMax - graphYMin;
    pixToXCoord = graphXRange/width;
    pixToYCoord = graphYRange/height;
}

static void pan (double xAmount, double yAmount)
{
    graphXMin += xAmount;
    graphXMax += xAmount;
    graphYMin += yAmount;
    graphYMax += yAmount;

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (graphXMin, graphXMax, graphYMin, graphYMax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void zoom (double xAmount, double yAmount)
{
    graphXMin -= xAmount;
    graphXMax += xAmount;
    graphYMin -= yAmount;
    graphYMax += yAmount;

    initializeViewMatrix ();
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (graphXMin, graphXMax, graphYMin, graphYMax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();   
}

void Simulator::radioCB (int val)
{
  switch (val){
    // heuristic mode modified
    case 1:
      callbackInstance->igv.pf.setHeuristic ((heuristic_type) heuristicMode );
    break;
  }
}

void Simulator::intBoxCB (int val)
{
  switch (val){
    case 1:
      callbackInstance->igv.pf.setDivisions (glui_divisions);
    break;
  }
}

void Simulator::floatBoxCB (int val)
{
  switch (val){
    case 1:
      callbackInstance->igv.pf.setRadius (glui_radius);
    break;

    case 2:
      callbackInstance->igv.pf.setWallDistance (glui_wall_distance);
    break;

    case 3:
      callbackInstance->igv.pf.setCrowdDistance (glui_crowd_distance);
    break;
    
    case 4:
      callbackInstance->igv.pf.setGoalDistance (glui_goal_distance);
    break;
  }
}

static void updateCamera (int val)
{
  if (keyMask['w']){
    pan (0, graphYRange/PAN_INC);
  }
  else if (keyMask['s']){
    pan (0, -graphYRange/PAN_INC);
  }
  
  if (keyMask['a']){
    pan (-graphXRange/PAN_INC, 0);
  }
  else if (keyMask['d']){
    pan (graphXRange/PAN_INC, 0);
  }

  if (keyMask['=']){
    zoom ( graphXRange/ZOOM_INC, graphYRange/ZOOM_INC );
  }
  else if (keyMask['-']){
    zoom ( -graphXRange/ZOOM_INC, -graphYRange/ZOOM_INC );
  }

  //glutTimerFunc (40, updateCamera, 0);
}

Simulator::Simulator (int argc, char** argv)
{
  //qtree = new QuadTree <int> (origin, axis, 1, 26);
	init (argc, argv);
  initGUI ();
}

Simulator::~Simulator ()
{

}

void Simulator::init (int argc, char** argv)
{
	glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize (width, height);
  gl_window_id = glutCreateWindow ("Autonomous Vehicle Simulator");
  glEnable( GL_DEPTH_TEST );
  glClearColor(0, 0, 0, 0);

  glutMotionFunc (motionWrapper);
  GLUI_Master.set_glutMouseFunc (mouseWrapper);
  glutTimerFunc (16, timerWrapper, 0);
  glutDisplayFunc(displayWrapper);
  GLUI_Master.set_glutReshapeFunc(reshapeWrapper);
  GLUI_Master.set_glutKeyboardFunc(keyboardWrapper);
  glutKeyboardUpFunc(keyboardUpWrapper);
  GLUI_Master.set_glutSpecialFunc(specialWrapper);
  glutSpecialUpFunc(specialUpWrapper);
  fill(keyMask, keyMask+255, 0);
}

void Simulator::initGUI ()
{
  glui = GLUI_Master.create_glui ( "Settings", 0, 200, 200);
  GLUI_Master.set_glutIdleFunc(NULL);
  
  glui->add_checkbox ("Display entire maze", &drawEnv, 1);
  glui->add_checkbox ("Display vehicle LOS", &drawIGV, 2);
  glui->add_checkbox ("Display vehicle map", &drawVisibleLines, 3);
  
  glui->add_button ("Reset Environment", 1, buttonCB);

  GLUI_Panel *heuristicPanel = glui->add_panel ("Search Heuristic");
    GLUI_RadioGroup* heuristicRadio = glui->add_radiogroup_to_panel (heuristicPanel, &heuristicMode, 1, radioCB);
      glui->add_radiobutton_to_group (heuristicRadio, "Breadth First");
      glui->add_radiobutton_to_group (heuristicRadio, "Greedy");
      glui->add_radiobutton_to_group (heuristicRadio, "Combined");

  glui->add_edittext ( "# Directions: ", GLUI_EDITTEXT_INT , &glui_divisions, 1, intBoxCB );

  glui->add_edittext ( "Radius: ", GLUI_EDITTEXT_FLOAT , &glui_radius, 1, floatBoxCB );
  glui->add_edittext ( "Min Wall Distance: ", GLUI_EDITTEXT_FLOAT , &glui_wall_distance, 2, floatBoxCB );
  glui->add_edittext ( "Min Crowd Distance: ", GLUI_EDITTEXT_FLOAT , &glui_crowd_distance, 3, floatBoxCB );
  glui->add_edittext ( "Min Goal Distance: ", GLUI_EDITTEXT_FLOAT , &glui_goal_distance, 4, floatBoxCB );
}

void Simulator::buttonCB (int button)
{
  switch (button){
    case 1:
      callbackInstance->reset ();
    break;
  }
}

void Simulator::reset ()
{
  igv.visibleLines.clear ();
  igv.env.clear ();
  igv.pf.clear ();
  igv.fullStop ();
  env.destroy ();
}

void Simulator::saveEnvironment ()
{
  ofstream fout ("course.env");
  env.printLineSegments (fout);
}

void Simulator::loadEnvironment ()
{
  ifstream fin ("course.env");
  string next_token("");
  fin >> next_token;
  while (fin.good()){
    if (next_token == "START_LINE"){
      string x(""), y("");
      fin >> x;
      while (x != "END_LINE"){
        fin >> y;
        vertex v (atof (x.c_str()), atof (y.c_str()));
        env.insertLineSegment(v);
        targetPoint.push_back (v);
        fin >> x;
      }
      env.endLineSegment();
    }
    fin >> next_token;
  }
}

void Simulator::updateIGV ()
{
  if (arrowKeys[ARROW_UP]){
    igv.forward (0.1);
  }
  else if (arrowKeys[ARROW_DOWN]){
    igv.forward (-0.1);
  }

  if (arrowKeys[ARROW_LEFT]){
    igv.rotate (2.0);
  }
  else if (arrowKeys[ARROW_RIGHT]){
    igv.rotate (-2.0);
  }

  if (keyMask['Z']){
    igv.setCameraRange (igv.getCameraRange()+0.03);
  }
  else if (keyMask['z']){
    igv.setCameraRange (igv.getCameraRange()-0.03);
  }

  if (keyMask['X']){
    igv.setCameraSpread (igv.getCameraSpread()+0.3);
  }
  else if (keyMask['x']){
    igv.setCameraSpread (igv.getCameraSpread()-0.3);
  }
  
  // simulates camera interface by checking environment for lines in IGV's
  // current line of sight
  setVisibleLines ();

  igv.updatePosition ();

  igv.runProgram ();

}

void Simulator::setVisibleLines ()
{
  vector <pair <vertex, Line*> > found;

  // get all points in rectangular region around the IGV
  found = env.lineMap->getObjectsInRegion (
      {igv.position.x-igv.getCameraRange(), igv.position.y-igv.getCameraRange()}, 
      {igv.position.x+igv.getCameraRange(), igv.position.y+igv.getCameraRange()});

  igv.visibleLines.clear();

  // add points which are in the IGV's camera's view
  for (int i=0; i < found.size(); ++i){
    if ( pointInCircleSlice (
        found[i].first, igv.position, 
        igv.cameraMinRange, igv.cameraMaxRange, 
        igv.rotation-(igv.cameraSpread/2.0),
        igv.rotation+(igv.cameraSpread/2.0) )
      ){
      igv.visibleLines.push_back( found[i].second ) ;
    }
  }
}

bool Simulator::pointInCircleSlice (
  const vertex& point, const vertex& circ,
  float minRad, float maxRad, 
  float minTheta, float maxTheta
){
  vertex vec(point.x-circ.x, point.y-circ.y);
  double dist = vec.distance ();
  if ( (dist >= minRad) && (dist <= maxRad) ){
    float theta = atan (abs(vec.y/vec.x)) * DEGREES_PER_RADIAN;
    if (point.x >= circ.x){
      if (point.y >= circ.y){
        theta = 270.0+theta;
      }else{
        theta = 270.0-theta;
      }
    }else{
      if (point.y >= circ.y){
        theta = 90.0-theta;
      }else{
        theta = theta+90.0;
      }
    }
    if ((theta >= minTheta) && (theta <= maxTheta)){
      return true;
    }
  }
  return false;
}


void Simulator::timer (int val)
{
  updateIGV ();
  updateCamera (val);
  glutPostRedisplay ();
  glui->sync_live();
	glutTimerFunc (18, timerWrapper, 0);
}

void Simulator::display()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  if (drawQuadTree){
    glColor3f (0, 0, 1);
    env.drawLineQuadTree ();
  }

  if (drawEnv){
    env.drawLineSegments ();
  }

  if (drawVisibleLines){
    glColor3f (0, 1, 0);
      glBegin (GL_LINES);
      
      for (int i=0; i < igv.env.lines.size(); ++i){
        if (igv.env.lines[i]->next != NULL)
        {
          glVertex2f (igv.env.lines[i]->start.x, igv.env.lines[i]->start.y);
          glVertex2f (igv.env.lines[i]->next->start.x, igv.env.lines[i]->next->start.y);
        }
        if (igv.env.lines[i]->prev != NULL)
        {
          glVertex2f (igv.env.lines[i]->start.x, igv.env.lines[i]->start.y);
          glVertex2f (igv.env.lines[i]->prev->start.x, igv.env.lines[i]->prev->start.y);
        }
      }
      glEnd();
  }

  igv.draw();
  igv.env.drawWaypoints ();

  if (drawIGV){
    for (int i=0; i < igv.visibleLines.size(); ++i){
      if (igv.visibleLines[i]->next != NULL)
      {
        glVertex2f (igv.visibleLines[i]->start.x, igv.visibleLines[i]->start.y);
        glVertex2f (igv.visibleLines[i]->next->start.x, igv.visibleLines[i]->next->start.y);
      }
    }
  
    glColor3f (1, 1, 0);
    if (igv.pf.done ()){
      igv.pf.drawPath ();
    }
    else{
      igv.pf.drawRoutes ();
    }
  }

  glutSwapBuffers();
}

void Simulator::reshape(int w, int h)
{
    width = w;
    height = h;
    initializeViewMatrix();
    glViewport (0,0,(GLsizei)width, (GLsizei)height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (graphXMin, graphXMax, graphYMin, graphYMax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Simulator::motion (int x, int y)
{
  vertex newpoint ( pix_coord_to_point (x, y) );

  if (leftMouseDown){
      if (env.insertEvenSpacedLine (newpoint, minLineGap)){
        targetPoint.push_back(newpoint);
      }
    }
    else if (rightMouseDown){

    }
}

void Simulator::mouse (int button, int state, int x, int y)
{
  vertex newpoint ( pix_coord_to_point (x, y) );

    switch (button){
      case GLUT_LEFT_BUTTON:
        switch (state){
          case GLUT_DOWN:
              leftMouseDown = 1;
              targetPoint.push_back(newpoint);
              env.insertLineSegment (newpoint);
          break;

          case GLUT_UP:
              leftMouseDown = 0;
              env.endLineSegment ();
          break;
        }
      break;
      case GLUT_RIGHT_BUTTON:
        switch (state){
          case GLUT_DOWN:
            rightMouseDown = 1;
            igv.env.insertWaypoint (newpoint);
          break;

          case GLUT_UP:
            rightMouseDown = 0;
          break;
        }
      break;
    }
}



void Simulator::keyboard(unsigned char key, int x, int y)
{
  keyMask[key] = true;
	switch (key){
      case 'k':
        drawQuadTree = !drawQuadTree;
      break;

      case 'p':
      case 'P':
        saveEnvironment ();
      break; 

      case 'l':
      case 'L':
        loadEnvironment ();
      break;     

      case 'i':
      case 'I':
        drawIGV = !drawIGV;
      break;

      case 'u':
      case 'U':
        drawEnv = !drawEnv;
      break;

      case 'j':
      case 'J':
        drawSelectionBox = !drawSelectionBox;
      break;

      case 'h':
      case 'H':
        drawVisibleLines = !drawVisibleLines;
      break; 

      case '~':
        reset ();
      break;

      case 'f':
      break;

      case 'r':
        igv.autonomousMode = !igv.autonomousMode;
        if (!igv.autonomousMode) {
          igv.fullStop ();
          igv.clearPath ();
        }
      break;

      case 't':
        igv.pf.clear ();
        igv.findPath (vertex(3.0, 3.0));
      break;

      case 'e':
        igv.pf.expand ();
      break;

      case 'g':
        igv.displayInfo ();
      break;
  }
}

void Simulator::keyboardUp(unsigned char key, int x, int y)
{
  keyMask[key] = false;
}

void Simulator::special (int key, int x, int y)
{
  switch (key){
    case GLUT_KEY_UP:
      arrowKeys[ARROW_UP] = true;
    break;

    case GLUT_KEY_DOWN:
      arrowKeys[ARROW_DOWN] = true;
    break;

    case GLUT_KEY_LEFT:
      arrowKeys[ARROW_LEFT] = true;
    break;

    case GLUT_KEY_RIGHT:
      arrowKeys[ARROW_RIGHT] = true;
    break;
  }
}

void Simulator::specialUp (int key, int x, int y)
{
  switch (key){
    case GLUT_KEY_UP:
      arrowKeys[ARROW_UP] = false;
    break;

    case GLUT_KEY_DOWN:
      arrowKeys[ARROW_DOWN] = false;
    break;

    case GLUT_KEY_LEFT:
      arrowKeys[ARROW_LEFT] = false;
    break;

    case GLUT_KEY_RIGHT:
      arrowKeys[ARROW_RIGHT] = false;
    break;
  }
}

void Simulator::specialWrapper (int key, int x, int y){
  callbackInstance->special (key, x, y);
}

void Simulator::specialUpWrapper (int key, int x, int y){
  callbackInstance->specialUp (key, x, y);
}

void Simulator::motionWrapper (int x, int y){
  callbackInstance->motion (x, y);
}

void Simulator::mouseWrapper(int button, int state, int x, int y){
  callbackInstance->mouse(button, state, x, y);
}

void Simulator::timerWrapper(int val){
	callbackInstance->timer(val);
}

void Simulator::displayWrapper(){
	callbackInstance->display();
}

void Simulator::reshapeWrapper(int width, int height){
	callbackInstance->reshape(width, height);
}

void Simulator::keyboardWrapper(unsigned char key, int x, int y){
	callbackInstance->keyboard(key, x, y);
}

void Simulator::keyboardUpWrapper (unsigned char key, int x, int y){
  callbackInstance->keyboardUp (key, x, y);
}

void Simulator::run ()
{
	if (callbackInstance == NULL){
    callbackInstance = this;
  }
	glutMainLoop();
}
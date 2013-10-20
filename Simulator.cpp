#include "Simulator.h"

static int height = 960;
static int width = 960;
static double ZOOM_INC = 128.0;
static double PAN_INC = 128.0;

static float graphXMin = -9;
static float graphXMax = 9;
static float graphXRange = graphXMax - graphXMin;
static float pixToXCoord = graphXRange/width;

static float graphYMin = -9;
static float graphYMax = 9;
static float graphYRange = graphYMax - graphYMin;
static float pixToYCoord = graphYRange/height;

static bool leftMouseDown = 0;
static bool rightMouseDown = 0;

static vertex squareCenter (0, 0);
static vertex squareRange (4, 4);

static vertex origin (0, 0);
static vertex axis (128.0, 128.0);

static vector <vertex> targetPoint;
static vector <vertex> foundPoint;
static vector <Line*> visibleLines;
static vector <vertex> visiblePoints;



static int bucketSize = 1;
//static QuadTree <int>* qtree;
static bool drawQuadTree = 0;

static double minLineGap = 0.1;

static int gl_window_id;
static int gui_window_id;

bool keyMask [255];
bool arrowKeys [4];

bool drawIGV = false;
bool drawEnv = true;
bool drawSelectionBox = false;
bool drawVisibleLines = true;

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

// static void findPoints ()
// {
//   vector <pair <vertex, int> > found;
//   found = qtree->getObjectsInRegion (
//       {squareCenter.x-squareRange.x, squareCenter.y-squareRange.y}, 
//       {squareCenter.x+squareRange.x, squareCenter.y+squareRange.y});

//   foundPoint.clear();
//   foundPoint.resize(found.size());

//   for (int i=0; i < found.size(); ++i){
//       foundPoint[i] = found[i].first;
//   }
// }

static void editModeCallBack (int val)
{

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

  glutTimerFunc (40, updateCamera, 0);
}

Simulator::Simulator (int argc, char** argv)
{
  //qtree = new QuadTree <int> (origin, axis, 1, 26);
	init (argc, argv);
}

Simulator::~Simulator ()
{

}

void Simulator::init (int argc, char** argv)
{
	glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize (width, height);
  //glutInitContextVersion( 3, 2 );
  //glutInitContextProfile( GLUT_CORE_PROFILE );
  gl_window_id = glutCreateWindow ("Autonomous Vehicle Simulator");
  //glutSetWindowTitle("");
  //glutSetIconTitle("");
  glEnable( GL_DEPTH_TEST );
  glClearColor(0, 0, 0, 0);

  glutMotionFunc (motionWrapper);
  glutMouseFunc (mouseWrapper);
  glutTimerFunc (16, timerWrapper, 0);
  glutTimerFunc (40, updateCamera, 0);
  glutDisplayFunc(displayWrapper);
  glutReshapeFunc(reshapeWrapper);
  glutKeyboardFunc(keyboardWrapper);
  glutKeyboardUpFunc(keyboardUpWrapper);
  glutSpecialFunc(specialWrapper);
  glutSpecialUpFunc(specialUpWrapper);
  fill(keyMask, keyMask+255, 0);

  initGUI ();
}

void Simulator::initGUI ()
{
  //gui_window_id = glutCreateWindow("Simulator Parameters");
  GLUI *glui = GLUI_Master.create_glui ( "Simulator Parameters", 0, 200, 200);
  glui->add_checkbox ("click me");
  glui->add_checkbox ("click me");
  glui->add_checkbox ("click me");
  glui->add_button ("I\'m a button");
  GLUI_Panel *panel = glui->add_panel ("Panel");
  glui->add_button_to_panel (panel, "Buttons");
  glui->add_column_to_panel (panel, true);
  glui->add_button_to_panel (panel, "Are");
  glui->add_column_to_panel (panel, true);
  glui->add_button_to_panel (panel, "Cool");
  GLUI_RadioGroup* displayMode = glui->add_radiogroup (NULL, 0, editModeCallBack);
  //GLUI_RadioButton editMode = glui->add_radiobutton_to_group (displayMode, "edit map");
  //GLUI_RadioButton editMode (displayMode, "edit map");
  //GLUI_RadioButton 
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
  
  setVisibleLines ();
}

void Simulator::setVisibleLines ()
{
  vector <pair <vertex, Line*> > found;
  found = env.lineMap->getObjectsInRegion (
      {igv.position.x-igv.getCameraRange(), igv.position.y-igv.getCameraRange()}, 
      {igv.position.x+igv.getCameraRange(), igv.position.y+igv.getCameraRange()});

  visibleLines.clear();
  visibleLines.resize(found.size());

  for (int i=0; i < found.size(); ++i){
      visibleLines[i] = found[i].second;
  }
  //glutTimerFunc (250, setVisibleLines, 0);
  //igv.setVisibleLines (found);
}

void Simulator::motion (int x, int y)
{
  vertex newpoint ( x*pixToXCoord + graphXMin, -y*pixToYCoord + graphYMax);

  if (leftMouseDown){
      if (env.insertEvenSpacedLine (newpoint, minLineGap))
      {
        targetPoint.push_back(newpoint);
      }
    }
    else if (rightMouseDown){
      squareCenter = newpoint;
      //findPoints ();
    }
    
    glutPostRedisplay();
}

void Simulator::mouse (int button, int state, int x, int y)
{
  vertex newpoint ( x*pixToXCoord + graphXMin, -y*pixToYCoord + graphYMax);

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
            squareCenter = newpoint;
            //findPoints ();
          break;

          case GLUT_UP:
            rightMouseDown = 0;
          break;
        }
      break;
    }
    glutPostRedisplay();
}


void Simulator::timer (int val)
{
  updateIGV ();
  //updateCamera ();
  glutPostRedisplay ();
	glutTimerFunc (18, timerWrapper, 0);
}

void Simulator::display()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  if (drawQuadTree){
    glColor3f (0, 0, 1);
    env.drawLineQuadTree ();
  }

  // glColor3f (0, 0, 1);
  // glPointSize (3.0);
  // glBegin (GL_POINTS);
  //     for (unsigned i=0; i<targetPoint.size(); ++i){
  //         glVertex2f (targetPoint[i].x, targetPoint[i].y);
  //     }
  // glEnd();

  if (drawEnv){
    env.drawLineSegments ();
  }

  if (drawSelectionBox){
    glColor3f (0, 1, 0);
      glBegin (GL_LINE_LOOP);
          glVertex2f (squareCenter.x-squareRange.x, squareCenter.y-squareRange.y);
          glVertex2f (squareCenter.x-squareRange.x, squareCenter.y+squareRange.y);
          glVertex2f (squareCenter.x+squareRange.x, squareCenter.y+squareRange.y);
          glVertex2f (squareCenter.x+squareRange.x, squareCenter.y-squareRange.y);
      glEnd();

    // found points 
    glColor3f (0, 1, 0);
    glPointSize (3.0);
    glBegin (GL_POINTS);
        for (unsigned i=0; i<foundPoint.size(); ++i){
            glVertex2f (foundPoint[i].x, foundPoint[i].y);
        }
    glEnd();
  }

  if (drawVisibleLines){
    glColor3f (0, 1, 0);
      glBegin (GL_LINES);
      for (int i=0; i < visibleLines.size(); ++i){
        if (visibleLines[i]->next != NULL)
        {
          glVertex2f (visibleLines[i]->start.x, visibleLines[i]->start.y);
          glVertex2f (visibleLines[i]->next->start.x, visibleLines[i]->next->start.y);
        }
      }
      glEnd();
  }

  if (drawIGV){
    igv.draw();
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

void Simulator::keyboard(unsigned char key, int x, int y)
{
  //cout << key << " pressed\n";
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
        env.destroy();
        targetPoint.clear();
      break;

      case 'f':
        //findPoints ();
      break;
  }
  glutPostRedisplay();
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
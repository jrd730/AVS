#include "Simulator.h"

static int height = 480;
static int width = 480;
static double ZOOM_INC = 32.0;
static double PAN_INC = 32.0;

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
static vertex squareRange (10, 10);

static vertex origin (0, 0);
static vertex axis (128.0, 128.0);

static vector <vertex> targetPoint;
static vector <vertex> foundPoint;

static int bucketSize = 1;
static QuadTree <int>* qtree;
static bool drawQuadTree = 1;

static double minLineGap = 0.1;

bool keyMask [255];

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

static void findPoints ()
{
  vector <pair <vertex, int> > found;
  found = qtree->getObjectsInRegion (
      {squareCenter.x-squareRange.x, squareCenter.y-squareRange.y}, 
      {squareCenter.x+squareRange.x, squareCenter.y+squareRange.y});

  foundPoint.clear();
  foundPoint.resize(found.size());

  for (int i=0; i < found.size(); ++i){
      foundPoint[i] = found[i].first;
  }
}

Simulator::Simulator (int argc, char** argv)
{
  qtree = new QuadTree <int> (origin, axis, 1, 16);
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
  glutCreateWindow ("Autonomous Vehicle Simulator");
  //glutSetWindowTitle("");
  //glutSetIconTitle("");
  glEnable( GL_DEPTH_TEST );
  glClearColor(0, 0, 0, 0);

  glutMotionFunc (motionWrapper);
  glutMouseFunc (mouseWrapper);
  glutTimerFunc (16, timerWrapper, 0);
  glutDisplayFunc(displayWrapper);
  glutReshapeFunc(reshapeWrapper);
  glutKeyboardFunc(keyboardWrapper);

  fill(keyMask, keyMask+255, 0);
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
        fin >> x;
      }
      env.endLineSegment();
    }
    fin >> next_token;
  }
}

void Simulator::motion (int x, int y)
{
  vertex newpoint ( x*pixToXCoord + graphXMin, -y*pixToYCoord + graphYMax);

  if (leftMouseDown){
      if (env.insertEvenSpacedLine (newpoint, minLineGap)){
        targetPoint.push_back(newpoint);
        qtree->insert (newpoint, 1);  
      }
    }
    else if (rightMouseDown){
      squareCenter = newpoint;
      findPoints ();
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
              qtree->insert (newpoint, 1);

              env.insertEvenSpacedLine (newpoint, 1.0);
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
            findPoints ();
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
  glutPostRedisplay ();
	glutTimerFunc (40, timerWrapper, 0);
}

void Simulator::display()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  if (drawQuadTree){
    glColor3f (1, 1, 1);
    //qtree->draw(); 
    env.drawLineQuadTree ();
  }

  // glColor3f (0, 0, 1);
  // glPointSize (3.0);
  // glBegin (GL_POINTS);
  //     for (unsigned i=0; i<targetPoint.size(); ++i){
  //         glVertex2f (targetPoint[i].x, targetPoint[i].y);
  //     }
  // glEnd();

  glColor3f (0, 1, 0);
    glBegin (GL_LINE_LOOP);
        glVertex2f (squareCenter.x-squareRange.x, squareCenter.y-squareRange.y);
        glVertex2f (squareCenter.x-squareRange.x, squareCenter.y+squareRange.y);
        glVertex2f (squareCenter.x+squareRange.x, squareCenter.y+squareRange.y);
        glVertex2f (squareCenter.x+squareRange.x, squareCenter.y-squareRange.y);
    glEnd();

  env.drawLineSegments ();

  // found points 
  glColor3f (0, 1, 0);
  glPointSize (3.0);
  glBegin (GL_POINTS);
      for (unsigned i=0; i<foundPoint.size(); ++i){
          glVertex2f (foundPoint[i].x, foundPoint[i].y);
      }
  glEnd();

  igv.draw();

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
  keyMask[key] = !keyMask[key];
	switch (key){
      case 'c':
      case 'C':
          
      break;

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

      case 'a':
      case 'A':
          pan (-graphXRange/PAN_INC, 0);
      break;

      case 'd':
      case 'D':
          pan (graphXRange/PAN_INC, 0);
      break; 

      case 'w':
      case 'W':
          pan (0, graphYRange/PAN_INC);
      break; 

      case 's':
      case 'S':
          pan (0, -graphYRange/PAN_INC);
      break;

      case 'r':
      case 'R':
          
      break;

      case 'b':
          
      break;

      case 'B':
          
      break;

      case '~':
        env.destroy();
        targetPoint.clear();

      break;

      case '+':
      case '=':
          zoom ( graphXRange/ZOOM_INC, graphYRange/ZOOM_INC );
      break; 

      case '-':
      case '_':
          zoom ( -graphXRange/ZOOM_INC, -graphYRange/ZOOM_INC );
      break;

      case 'f':
        findPoints ();
      break;
  }
  glutPostRedisplay();
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

void Simulator::run ()
{
	if (callbackInstance == NULL){
    callbackInstance = this;
  }
	glutMainLoop();
}
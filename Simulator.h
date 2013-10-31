/*
Simulator places a simulated IGV in a generated environment, allowing it to move about freely. In the abscence of actual camera and GPS data, the simulator
object feeds the IGV's sensors, so that the IGV can map out its environment.
*/
#pragma once
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <GL/glui.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <sstream>
#include <string> 

#include "Environment.h"
#include "IGV.h"

enum INSERT_MODE{
	INSERT_LINE, INSERT_POLYGON
};

enum ARROW_ID {
  ARROW_UP = 0, ARROW_RIGHT = 1, ARROW_DOWN = 2, ARROW_LEFT = 3
};

class Simulator
{
	public:

		Simulator (int argc,char** arg);
		~Simulator ();

		void init (int argc, char** argv);
		void run ();

	private:

		Environment env;
		IGV igv;

		static Simulator* callbackInstance;

    void initGUI ();

    void saveEnvironment ();
    
    void loadEnvironment ();

    void updateIGV ();
    
    void setVisibleLines ();
    
    bool pointInCircleSlice (
      const vertex& point, const vertex& circ,
      float minRad, float maxRad, 
      float minTheta, float maxTheta
    );

    //void updateCamera ();

    void special (int, int, int);
    void specialUp (int, int, int);
    void motion (int, int);
    void mouse (int, int, int, int);
    void timer (int);
    void display();
    void reshape(int, int);
    void keyboard(unsigned char, int, int);
    void keyboardUp(unsigned char, int, int);

    static void specialWrapper (int, int, int);
    static void specialUpWrapper (int, int, int);
  	static void motionWrapper (int, int);
  	static void mouseWrapper (int, int, int, int);
  	static void timerWrapper(int);
    static void displayWrapper();
    static void reshapeWrapper(int, int);
    static void keyboardWrapper(unsigned char, int, int);
    static void keyboardUpWrapper (unsigned char, int, int);
};
/*
Simulator places a simulated IGV in a generated environment, allowing it to move about freely. In the abscence of actual camera and GPS data, the simulator
object feeds the IGV's sensors, so that the IGV can map out its environment.
*/

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <sstream>
#include <string> 

#include "Environment.h"
#include "IGV.h"

enum INSERT_MODE{
	INSERT_LINE, INSERT_BARREL
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

		static void motionWrapper (int, int);
		static void mouseWrapper (int, int, int, int);
		static void timerWrapper(int);
    static void displayWrapper();
    static void reshapeWrapper(int, int);
    static void keyboardWrapper(unsigned char, int, int);

    void motion (int, int);
    void mouse (int, int, int, int);
    void timer (int);
    void display();
    void reshape(int, int);
    void keyboard(unsigned char, int, int);
};
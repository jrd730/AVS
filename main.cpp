//#include <GL/glfw.h>
//#include <GLFW/glfw3.h>
#include <iostream>
#include "Simulator.h"

using namespace std;

int main (int argc, char** argv)
{
	/*
	if (!glfwInit()){
		cout << "derp\n";
		return 0;
	}
	
	
	GLFWwindow* window = glfwCreateWindow(640, 480, "Cool-Ass Window", NULL, NULL);
	//GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", glfwGetPrimaryMonitor(), NULL); // full screen
	
	
	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window)){
	// Keep running
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	*/

	Simulator simulator(argc, argv);
	simulator.run();

	return 0;
}
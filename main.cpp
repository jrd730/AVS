#include <iostream>
#include "Simulator.h"
using namespace std;

int main (int argc, char** argv)
{
	Simulator simulator(argc, argv);
	simulator.run();
	return 0;
}
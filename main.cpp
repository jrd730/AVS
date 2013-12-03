#include <iostream>
#include "Simulator.h"
using namespace std;

int main (int argc, char** argv)
{
  vertex v1 (0, 0);
  vertex v2 (1, 0);
  vertex v3 (1, 1);
  vertex v4 (0, 1);
  vertex v5 (-1, 1);
  vertex v6 (-1, 0);
  vertex v7 (-1, -1);
  vertex v8 (0, -1);
  vertex v9 (1, -1);

  cout << v1.angleTo (v2) << endl;
  cout << v1.angleTo (v3) << endl;
  cout << v1.angleTo (v4) << endl;
  cout << v1.angleTo (v5) << endl;
  cout << v1.angleTo (v6) << endl;
  cout << v1.angleTo (v7) << endl;
  cout << v1.angleTo (v8) << endl;
  cout << v1.angleTo (v9) << endl;

	Simulator simulator(argc, argv);
	simulator.run();
	return 0;
}
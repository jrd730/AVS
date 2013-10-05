#include "QuadTree.h"
#include "Lane.h"

class Environment
{
	public:
		Environment ();
		~Environment ();

	private:

		QuadTree <Lane*>* lanes;
};
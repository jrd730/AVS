#include "Pathfinder.h"

static vertex origin (0, 0);
static vertex axis (128.0, 128.0);

Pathfinder::Pathfinder ()
{
  root = NULL;
  max_expansions = 10;
}

Pathfinder::~Pathfinder ()
{
  clear ();
}

void Pathfinder::clear ()
{
  if (root)
    delete root;

  root = NULL;

  while (!q.empty()) q.pop ();
}

void Pathfinder::set (int div, float rad, float cd, float wb)
{
  divisions = div;
  radius = rad;
  crowded_dist = cd;
  wall_buf = wb;
}

void Pathfinder::begin (QuadTree <Line*>* w, vertex s, vertex e)
{
  walls = w;
  start = s;
  end = e;
  root = new PathNode (s, NULL, divisions);
  q.push (root);

  visited =  new QuadTree <PathNode*> (origin, axis, 1, 26);
}

bool Pathfinder::canExpand (vertex v)
{
  cout << "checking for walls...";
  vector <pair <vertex, Line*> > blockingWalls = walls->getObjectsInRegion (v-wall_buf, v+wall_buf);
  if (blockingWalls.size () > 0) return false;
  cout << "complete\n";

  cout << "checking for crowding...";
  vector <pair <vertex, PathNode*> > traversed = visited->getObjectsInRegion (v-crowded_dist, v+crowded_dist);
  if (traversed.size () > 0) return false;
  cout << "complete\n";

  return true;

}

bool Pathfinder::expand ()
{
  float theta = 360.0 / (float)divisions;
  vertex v[divisions];
  
  cout << "setting expansion intervals...";
  for (int i = 0; i < divisions; ++i){
    v[i].x = radius * cos ( (theta*i)/DEGREES_PER_RADIAN );
    v[i].y = radius * sin ( (theta*i)/DEGREES_PER_RADIAN );
  }
  cout << "complete\n";

  int num_expansions = 0;
  while ( !q.empty() && (num_expansions < max_expansions) )
  {
    list <PathNode*> newNodes;
    for (int i = 0; i < divisions; ++i)
    {
      PathNode* top = q.top();
      vertex expansionPoint = top->pos + v[i];
      if (canExpand (expansionPoint) )
      {
        cout << "able to expand\n";
        float weight = expansionPoint.distance (end);
        if (weight < 0.5) found_path = true;

        PathNode* newNode = new PathNode (expansionPoint, top, divisions, weight);
        visited->insert (expansionPoint, newNode);
        newNodes.push_back (newNode);
        top->next[i] = newNode;
        num_expansions++;
      }
      else{
        cout << "unable to expand\n";
      }
    }
    q.pop ();
    
    for (; !newNodes.empty(); newNodes.pop_front ()){
      q.push (newNodes.front ());
    }
  }

  return (num_expansions > 0);
}

bool Pathfinder::done ()
{
  return found_path;
}

void Pathfinder::draw ()
{
  if (root){
    draw (root);
  }
}

void Pathfinder::draw (PathNode* p)
{
  for (int i = 0; i < divisions; ++i)
  {
    if (p->next[i]){
      glBegin (GL_LINES);
        glVertex2f (p->pos.x, p->pos.y);
        glVertex2f (p->next[i]->pos.x, p->next[i]->pos.y);
      glEnd ();

      draw (p->next[i]);
    }
  }
}
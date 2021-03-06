#include "Pathfinder.h"

static vertex origin (0, 0);
static vertex axis (128.0, 128.0);

Pathfinder::Pathfinder ()
{
  found_path = false;
  impossible = false;
  working = false;
  root = NULL;
  max_expansions = 100;
  visited = NULL;
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
  path.clear ();
  
  found_path = false;
  working = false;
}

void Pathfinder::set (int div, float rad, float cd, float wb, float nb)
{
  radius = rad;
  crowded_dist = cd;
  wall_buf = wb;
  node_buf = nb;
  setDivisions (div);
}

void Pathfinder::setDivisions (int d)
{
  divisions = d;
  float theta = 360.0 / (float)divisions;
  v.resize (divisions);
  
  for (int i = 0; i < divisions; ++i){
    v[i].x = radius * cos ( (theta*i)/DEGREES_PER_RADIAN );
    v[i].y = radius * sin ( (theta*i)/DEGREES_PER_RADIAN );
  }
}
  
void Pathfinder::setRadius (float r)
{
  radius = r;
}

void Pathfinder::setCrowdDistance (float cd)
{
  crowded_dist = cd;
}

void Pathfinder::setWallDistance (float wb)
{
  wall_buf = wb;
}

void Pathfinder::setGoalDistance (float nb)
{
  node_buf = nb;
}

float Pathfinder::getDivisions ()
{
  return divisions;
}
    
float Pathfinder::getRadius ()
{
  return radius;  
}

float Pathfinder::getCrowdDistance ()
{
  return crowded_dist;
}

float Pathfinder::getWallDistance ()
{
  return wall_buf;
}

float Pathfinder::getGoalDistance ()
{
  return node_buf;
}

void Pathfinder::begin (QuadTree <Line*>* w, vertex s, vertex e)
{
  found_path = false;
  impossible = false;
  working = true;
  walls = w;
  start = s;
  end = e;
  root = new PathNode (s, NULL, divisions);
  q.push (root);

  if (visited != NULL){
    delete visited;
  }
  visited =  new QuadTree <PathNode*> (origin, axis, 1, 26);
}

bool Pathfinder::canExpand (vertex v)
{
  vector <pair <vertex, Line*> > blockingWalls = walls->getObjectsInRegion (v-wall_buf, v+wall_buf);
  if (blockingWalls.size () > 0) return false;

  vector <pair <vertex, PathNode*> > traversed = visited->getObjectsInRegion (v-crowded_dist, v+crowded_dist);
  if (traversed.size () > 0) return false;

  return true;
}

void Pathfinder::setPath (PathNode* p)
{
  for (; p; p=p->prev){
    if (p != NULL){
      path.push_front (p);
    }
  }
}

bool Pathfinder::expand ()
{
  int num_expansions = 0;
  while ( !q.empty() && (num_expansions < max_expansions) && !found_path)
  {
    list <PathNode*> newNodes;
    for (int i = 0; i < divisions; ++i)
    {
      PathNode* top = q.top();
      vertex expansionPoint = top->pos + v[i];
      if (canExpand (expansionPoint) )
      {
        PathNode* newNode = new PathNode (expansionPoint, top, divisions);
        
        newNode->dist_to = top->dist_to + radius;
        newNode->dist_remaining = expansionPoint.distance (end);
        newNode->setWeight (start, end);
        
        visited->insert (expansionPoint, newNode);
        newNodes.push_back (newNode);
        
        top->next[i] = newNode;
        num_expansions++;

        if (newNode->dist_remaining < node_buf)
        {
          found_path = true;
          setPath (newNode);
          return true;
        }
      }
    }
    
    q.pop ();
    for (; !newNodes.empty(); newNodes.pop_front ()){
      q.push (newNodes.front ());
    }
  }

  if (q.empty () && !found_path){
    impossible = true;
  }

  return (num_expansions > 0);
}

vertex Pathfinder::getCurPathNode ()
{
  if (!path.empty()){
    return path.front ()->pos;
  }
}

bool Pathfinder::setNextPathNode ()
{
  if (path.empty ()) 
    return false;
  else{ 
    path.pop_front ();
    return true;
  }
}

bool Pathfinder::pathImpossible ()
{
  return impossible;
}

bool Pathfinder::endOfPath ()
{
  return path.empty();
}

bool Pathfinder::searching ()
{
  return working;
}

bool Pathfinder::done ()
{
  return found_path;
}

void Pathfinder::setHeuristic (heuristic_type ht)
{
  cur_heuristic = ht;
}

void Pathfinder::drawRoutes ()
{
  if (root){
    draw (root);
  }
}

void Pathfinder::drawPath ()
{
  list <PathNode*> :: iterator it;
  glBegin (GL_LINE_STRIP);
  for (it = path.begin(); it != path.end(); it++){
    glVertex2f ( (*it)->pos.x, (*it)->pos.y);
  }
  glEnd ();
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
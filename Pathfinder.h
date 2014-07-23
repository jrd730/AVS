
// finds the shortest path to a point from a starting point

#pragma once
#include "QuadTree.h"
#include "Line.h"
#include <cmath>
#include <iostream>
#include <list>
#include <queue>
#define DEGREES_PER_RADIAN 57.2957795
using namespace std;

enum heuristic_type{
  BREADTH_FIRST, GREEDY, COMBINED, ASTAR
};

static heuristic_type cur_heuristic = BREADTH_FIRST;

class PathNode
{
public:
  PathNode (vertex v, PathNode* p, int children, float w=0)
  {
    pos = v;
    prev = p;
    num_children = children;
    next = new PathNode* [num_children];
    for (int i = 0; i < num_children; ++i){
      next[i] = NULL;
    }
    weight = w;
    dist_to = 0;
  }

  ~PathNode ()
  {
    for (int i = 0; i < num_children; ++i){
      if (next[i]){
        delete next[i];
      }
    }
    delete [] next;
  }

  // weight is the combined heuristic value for choosing which nodes to expand 
  void setWeight (const vertex& start, const vertex& end)
  {
    switch (cur_heuristic)
    {
      // greedy best-first heuristic selects for nodes which have a closer
      // straight line distance to the end. Typically runs faster than breadth
      // first but often chooses poor paths.
      case GREEDY:
      weight = dist_remaining;
      break;

      // breadth first heuristic selects for minimum total distance traveled
      // guarantees finding the shortest path but takes the longest to run
      case BREADTH_FIRST:
      weight = dist_to;
      break;

      // a heuristic which combines elements of breadth first and greedy search
      // runs faster than BFS and with better results than greedy
      case COMBINED:
      weight = 0.5 * pow (dist_to, 2) + dist_remaining;
      break;

      // prioritize path nodes that lie in the bounding box formed by 
      // the start and end points
      case ASTAR:
      weight = dist_to - 100.0 * float ( 
        (start.x - end.x < 0) == (start.x-pos.x < 0) && 
        (start.y - end.y < 0) == (start.y-pos.y < 0));     
      break;
    }
  }

  bool operator < (PathNode p) const{
    return weight > p.weight;
  }

  vertex pos;
  PathNode* prev;
  PathNode** next;
  int num_children;
  float dist_to;
  float dist_remaining;
  float weight;
};

struct CompareNodes : public std::binary_function<PathNode*, PathNode*, bool> 
{
  bool operator () (PathNode* l, PathNode* r){
    return l->weight > r->weight;
  }
};

class Pathfinder
{
  public:
    
    Pathfinder ();

    ~Pathfinder ();

    void clear ();

    void set (int divisions, float radius, float crowded_dist, float wall_buf, float node_buf);
    
    void setDivisions (int divisions);
    
    void setRadius (float radius);
    
    void setCrowdDistance (float crowded_dist);
    
    void setWallDistance (float wall_buf);
    
    void setGoalDistance (float node_buf);

    float getDivisions ();
    
    float getRadius ();
    
    float getCrowdDistance ();
    
    float getWallDistance ();
    
    float getGoalDistance ();

    void begin ( QuadTree <Line*>* w, vertex s, vertex e);

    bool expand ();

    void depth ();

    bool searching ();

    bool done ();

    void drawRoutes ();
    
    void drawPath ();

    void setHeuristic (heuristic_type ht);

    vertex getCurPathNode ();

    bool setNextPathNode ();

    bool pathImpossible ();

    bool endOfPath ();

    list <PathNode*> path;
  
  private:

    bool canExpand (vertex v);
    void setPath (PathNode* p);
    void draw (PathNode* p);

    PathNode* root;

    bool working;
    bool found_path;
    bool impossible;
    int cur_depth;

    priority_queue <PathNode*, vector <PathNode*>, CompareNodes > q;

    vector <vertex> v;

    QuadTree <PathNode*>* visited;
    QuadTree <Line*>* walls;

    float crowded_dist;
    float wall_buf;
    float node_buf;
    float radius;

    int divisions;

    int max_expansions;

    vertex start;
    vertex end;
    vertex minXY;
    vertex maxXY;

};
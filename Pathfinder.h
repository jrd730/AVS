
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
  BREADTH_FIRST, GREEDY, COMBINED
};

static heuristic_type cur_heuristic = GREEDY;

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
  void setWeight ()
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

    void set (int divisions, float radius, float crowded_dist, float wall_buf); 

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

  private:

    bool canExpand (vertex v);
    void setPath (PathNode* p);
    void draw (PathNode* p);

    PathNode* root;

    list <PathNode*> path;

    bool working;
    bool found_path;
    int cur_depth;

    priority_queue <PathNode*, vector <PathNode*>, CompareNodes > q;

    vector <vertex> v;

    QuadTree <PathNode*>* visited;
    QuadTree <Line*>* walls;

    float crowded_dist;
    float wall_buf;
    float radius;

    int divisions;

    int max_expansions;

    vertex start;
    vertex end;

};
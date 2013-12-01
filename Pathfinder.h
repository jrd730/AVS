
// finds the shortest path to a point from a starting point

#pragma once
#include "QuadTree.h"
#include "Line.h"
#include <iostream>
#include <list>
#include <queue>
#define DEGREES_PER_RADIAN 57.2957795
using namespace std;

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

  bool operator < (PathNode p) const{
    return weight > p.weight;
  }

  vertex pos;
  PathNode* prev;
  PathNode** next;
  int num_children;
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

    bool done ();

    void draw ();

  private:

    bool canExpand (vertex v);
    void draw (PathNode* p);

    PathNode* root;

    bool found_path;
    int cur_depth;

    priority_queue <PathNode*, vector <PathNode*>, CompareNodes > q;

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
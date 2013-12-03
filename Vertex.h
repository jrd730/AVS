#pragma once
#include <cmath>
#include <iostream>
#define DEGREES_PER_RADIAN 57.2957795
class vertex
{
  public:

    vertex () : x(0), y(0){}

    vertex (long double newX, long double newY): x (newX), y(newY){}
    
    ~vertex (){}

    double distance (const vertex& v){
      double dx = x-v.x, dy = y-v.y;    
      return sqrt (dx*dx + dy*dy);
    }
    
    double distance (){
      return sqrt (x*x + y*y);
    }

    // returns the angle to another vertex, an angle of 0 is along the positive
    // x-axis, angles increase in a counterclockwise direction up to a maximum
    // of 360 degrees
    double angleTo (const vertex& v){
      float dy = (v.y-y);
      float dx = (v.x-x);

      // check for possible nan condition
      if (fabs (dy) < 0.000001){
        if (dx > 0){
          return 0;
        }
        else{
          return 180.0;
        }
      }
      else if (fabs (dx) < 0.000001){
        if (dy > 0){
          return 90.0;
        }
        else{
          return 270.0;
        }
      }

      else{
        float theta = 0;
        if (dx < 0){
          if (dy < 0){
            theta += 180.0;
          }
          else {
            theta += 90.0;
          }
        }
        else {
          if (dy < 0){
            theta += 270.0;
          }
          else {
            theta += 0;
          }
        }
        theta += atan ( fabs(dy) / fabs(dx) ) * DEGREES_PER_RADIAN;
        return theta;
      }
    }

    long double x;
    long double y;

    vertex operator *(float m) {return vertex (x*m, y*m);}
    vertex operator /(float m) {return vertex (x/m, y/m);}
    vertex operator +(float m) {return vertex (x+m, y+m);}
    vertex operator -(float m) {return vertex (x-m, y-m);}
    vertex operator +(vertex v){return vertex (x+v.x, y+v.y);}
    vertex operator -(vertex v){return vertex (x-v.x, y-v.y);}
    bool operator == (vertex v){ return ((x == v.x)&&(y==v.y));}
    friend std::ostream& operator << ( std::ostream& os, const vertex& v ){
        return os << "(" << v.x << ", " << v.y << ")";
    }
};

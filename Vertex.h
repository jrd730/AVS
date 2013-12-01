#pragma once
#include <cmath>

class vertex
{
    public:

        vertex () : x(0), y(0){}

        vertex (long double newX, long double newY): x (newX), y(newY){}
        
        ~vertex (){}

        double distance (vertex v){
            double dx = x-v.x, dy = y-v.y;    
            return sqrt (dx*dx + dy*dy);
        }
        
        double distance (){
            return sqrt (x*x + y*y);
        }

        long double x;
        long double y;

        vertex operator *(float m) {return vertex (x*m, y*m);}
        vertex operator /(float m) {return vertex (x/m, y/m);}
        vertex operator +(float m) {return vertex (x+m, y+m);}
        vertex operator -(float m) {return vertex (x-m, y-m);}
        vertex operator +(vertex v){return vertex (x+v.x, y+v.y);}
        vertex operator -(vertex v){return vertex (x-v.x, y-v.y);}
        bool operator == (vertex v){ return ((x == v.x)&&(y==v.y)); }
};

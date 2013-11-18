#include "DDV.h"

DDV::DDV ():
  position({0, 0}), rotation (0),
  fwdSpd (0), rteSpd (0)
{

}

DDV::~DDV ()
{
	
}

void DDV::rotate (double amount)
{
  rotation += amount;
  if (rotation > 360.0){
    rotation -= 360.0;
  }else if (rotation < 0){
    rotation += 360.0;
  }
}

void DDV::setRotation (double r)
{
  rotation = r;
}

void DDV::forward (double amount)
{
  position.x += amount * -sin (rotation/DEGREES_PER_RADIAN);
  position.y += amount * cos (rotation/DEGREES_PER_RADIAN);
}

void DDV::translate  (vertex amount)
{
  position.x += amount.x;
  position.y += amount.y;
}

void DDV::setPosition (vertex pos)
{
  position = pos;
}

void DDV::setForwardSpeed (double amt)
{
  fwdSpd = amt;
}

void DDV::setRotateSpeed (double amt)
{
  rteSpd = amt;
}

void DDV::fullStop ()
{
  setForwardSpeed (0);
  setRotateSpeed (0);
}

void DDV::updatePosition ()
{
  forward (fwdSpd * BASE_FORWARD_SPEED);
  rotate (rteSpd * BASE_ROTATE_SPEED);
}
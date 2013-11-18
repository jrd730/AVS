/*

DDV = differential drive vehicle

A differential drive vehicle is a vehicle with independent motion of its left and right side wheels. The movement of the vehicle can be described by
the ratio of angular velocities of each wheel. 
*/
#pragma once
#include <cmath>
#include "Vertex.h"
#define DEGREES_PER_RADIAN 57.2957795
#define BASE_FORWARD_SPEED 0.1
#define BASE_ROTATE_SPEED 1.0

class DDV
{
public:
	DDV();
	~DDV();

	void rotate (double amount);
	void setRotation (double r);

	void forward (double amount);
	void translate  (vertex amount);
	void setPosition (vertex pos);

	void setForwardSpeed (double amt);
	void setRotateSpeed (double amt);
	void fullStop ();

	void updatePosition ();
	
	vertex position;
	double rotation; // zero is facing north

	double fwdSpd;
	double rteSpd;
};
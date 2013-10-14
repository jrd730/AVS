/*

DDV = differential drive vehicle

A differential drive vehicle is a vehicle with independent motion of its left and right side wheels. The movement of the vehicle can be described by
the ratio of angular velocities of each wheel. 
*/
#pragma once
#include <cmath>
#include "Vertex.h"
#define DEGREES_PER_RADIAN 57.2957795
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

	void reposition (double dt);
	
	vertex position;
	double rotation; // zero is facing north
	double leftMotorSpeed;
	double rightMotorSpeed;

	// these variables can be used in conjunction with 
	// left and right motor speeds to come up with an 
	// estimate of the bot's updated position and 
	// direction after an indicated time slice
	double axleWidth;
	double wheelRadius;
	double wheelPerimeter;

};
/*

DDV = differential drive vehicle

A differential drive vehicle is a vehicle with independent motion of its left and right side wheels. The movement of the vehicle can be described by
the ratio of angular velocities of each wheel. 
*/
#pragma once
#include "Vertex.h"

class DDV
{
public:
	DDV();
	~DDV();

	void reposition (double dt);

private:

	
	vertex position;
	double direction;
	double leftMotorSpeed;
	double rightMotorSpeed;

	double axleWidth;
	double wheelRadius;
	double wheelPerimeter;

};
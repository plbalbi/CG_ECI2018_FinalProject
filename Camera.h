#pragma once

#include "stdafx.h"

#ifndef CAMERA_H
#define CAMERA_H

using namespace DirectX;

class Camera {
public:

	Camera();
	~Camera();

	Camera(XMVECTOR anEyePoint, XMVECTOR aToDirection, XMVECTOR anUpDirection) :
		eye(anEyePoint), to(aToDirection), up(anUpDirection) {
		// Create a lateral direction vector, in order to move camera position sideways
		setLateralDirectionFromTo(aToDirection);
	}

	const XMVECTOR getLateralDirection() { return lateralDirection; }

	XMVECTOR eye, to, up;

	XMMATRIX cameraMatrix();

	// Performs the rotation of the camera in the Y axis
	// Updates the 'to' and 'side' direction vectors
	void rotateCamera(float anAngle);

	// Translates the camera by a given displacement
	void translateCamera(XMVECTOR aDisplacement);

private:

	XMVECTOR lateralDirection;
	void setLateralDirectionFromTo(const DirectX::XMVECTOR &aToDirection);
};

#endif // !
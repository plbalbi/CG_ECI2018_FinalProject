#pragma once

#include "stdafx.h"

using namespace DirectX;

class Camera {
public:

	Camera(XMVECTOR anEyePoint, XMVECTOR aToDirection, XMVECTOR anUpDirection) :
		eye(anEyePoint), to(aToDirection), up(anUpDirection) {
	}

	XMVECTOR eye, to, up;

	XMMATRIX cameraMatrix();

	Camera();
	~Camera();
};


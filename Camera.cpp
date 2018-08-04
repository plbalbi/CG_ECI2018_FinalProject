#include "stdafx.h"
#include "Camera.h"


XMMATRIX Camera::cameraMatrix() {
	return XMMatrixLookToLH(
		this->eye,
		this->to,
		this->up
	);
}

Camera::Camera() {
}


Camera::~Camera() {
}

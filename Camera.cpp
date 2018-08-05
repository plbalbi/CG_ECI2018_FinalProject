#include "Camera.h"

XMMATRIX Camera::cameraMatrix() {
	return XMMatrixLookToLH(
		this->eye,
		this->to,
		this->up
	);
}

void Camera::rotateCamera(float anAngle) {
	this->to = XMVector3Transform(this->to, XMMatrixRotationY(anAngle));
	this->setLateralDirectionFromTo(this->to);
}

void Camera::translateCamera(XMVECTOR aDisplacement) {
	this->eye += aDisplacement;
}

void Camera::setLateralDirectionFromTo(const DirectX::XMVECTOR &aToDirection) {
	this->lateralDirection = XMVector3Transform(aToDirection, XMMatrixRotationY(XMConvertToRadians(90)));
}

Camera::Camera() {
}

Camera::~Camera() {
}

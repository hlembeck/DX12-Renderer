#include "Camera.h"

Camera::Camera() {
	m_position = { 0.0f,0.0f,-5.0f };
	m_right = { 1.0f,0.0f,0.0f };
	m_up = { 0.0f,1.0f,0.0f };
	m_direction = { 0.0f,0.0f,1.0f };
	SetLens(45.0f, 1.0f, 0.1f, 1.0f);
	UpdateViewMatrix();
}

Camera::Camera(float fovY, float aspectRatio, float nearZ, float farZ) {
	m_position = { 0.0f,0.0f,-5.0f };
	m_right = { 1.0f,0.0f,0.0f };
	m_up = { 0.0f,1.0f,0.0f };
	m_direction = { 0.0f,0.0f,1.0f };
	SetLens(fovY, aspectRatio, nearZ, farZ);
	UpdateViewMatrix();
}

Camera::~Camera() {

}

void Camera::SetLens(float fovY, float aspectRatio, float nearZ, float farZ) {
	m_fovY = fovY;
	m_aspectRatio = aspectRatio;
	m_nearZ = nearZ;
	m_farZ = farZ;

	m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
	m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f * m_fovY);

	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fovY, m_aspectRatio, m_nearZ, m_farZ); //Refers to the correct matrix in Luna's book (p189)


	/*XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, m_projectionMatrix);
	for (int i = 0; i < 4; i++) {
		printf("%g %g %g %g\n", mat.m[i][0], mat.m[i][1], mat.m[i][2], mat.m[i][3]);
	}*/ //To verify that the correct matrix is produced
}

void Camera::MoveForward(float dist) {
	m_position.x += m_direction.x * dist;
	m_position.y += m_direction.y * dist;
	m_position.z += m_direction.z * dist;
	UpdateViewMatrix();
}

void Camera::MoveSide(float dist) {
	m_position.x += m_right.x * dist;
	m_position.y += m_right.y * dist;
	m_position.z += m_right.z * dist;
	UpdateViewMatrix();
}

void Camera::Move(XMFLOAT3 dist) {
	m_position.x += m_right.x * dist.x + m_up.x * dist.y + m_direction.x * dist.z;
	m_position.y += m_right.y * dist.x + m_up.y * dist.y + m_direction.y * dist.z;
	m_position.z += m_right.z * dist.x + m_up.z * dist.y + m_direction.z * dist.z;
	UpdateViewMatrix();
}

void Camera::Pitch(float angle) {
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotationMatrix));
	XMStoreFloat3(&m_direction, XMVector3TransformNormal(XMLoadFloat3(&m_direction), rotationMatrix));
}

void Camera::RotateWorldY(float angle) {
	XMMATRIX rotationMatrix = XMMatrixRotationY(angle);
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotationMatrix));
	XMStoreFloat3(&m_direction, XMVector3TransformNormal(XMLoadFloat3(&m_direction), rotationMatrix));
}

void Camera::UpdateViewMatrix() {
	m_right = normalize(m_right);
	m_up = normalize(m_up);
	m_direction = normalize(m_direction);

	XMVECTOR right = XMLoadFloat3(&m_right);
	XMVECTOR up = XMLoadFloat3(&m_up);
	XMVECTOR direction = XMLoadFloat3(&m_direction);
	XMVECTOR position = XMLoadFloat3(&m_position);

	direction = XMVector3Normalize(direction);
	up = XMVector3Normalize(XMVector3Cross(direction, right));
	right = XMVector3Cross(up, direction);

	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, up);
	XMStoreFloat3(&m_direction, direction);

	float x = -XMVectorGetX(XMVector3Dot(position, right));
	float y = -XMVectorGetX(XMVector3Dot(position, up));
	float z = -XMVectorGetX(XMVector3Dot(position, direction));

	XMFLOAT4X4 view = {
		m_right.x, m_up.x, m_direction.x, 0.0f,
		m_right.y, m_up.y, m_direction.y, 0.0f,
		m_right.z, m_up.z, m_direction.z, 0.0f,
		x, y, z, 1.0f,
	};
	m_viewMatrix = XMLoadFloat4x4(&view);
}

XMMATRIX Camera::GetViewMatrix() {
	return m_viewMatrix;
}

XMMATRIX Camera::GetViewProjectionMatrix() {
	return XMMatrixMultiply(m_viewMatrix,m_projectionMatrix);
}

XMFLOAT4 Camera::GetPosition() {
	return { m_position.x, m_position.y, m_position.z, 1.0f };
}

XMFLOAT4 Camera::GetDirection() {
	XMFLOAT4 direction = { m_direction.x, m_direction.y, m_direction.z, 0.0f };
	return normalize(direction);
}

void Camera::MoveUp(float dist) {
	m_position.y += dist;
	UpdateViewMatrix();
}
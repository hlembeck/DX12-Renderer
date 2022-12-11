#include "Player.h"

Player::Player() {}

Player::~Player() {}

void Player::OnInit(float fovY, float aspectRatio, float nearZ, float farZ) {
	Camera::OnInit(fovY, aspectRatio, nearZ, farZ);
}

CameraShaderConstants Player::GetCameraConstants() {
    return { XMMatrixTranspose(Camera::GetViewProjectionMatrix()), Camera::GetDirection(), Camera::GetPosition() };
}

void Player::OnKeyDown(WPARAM wParam) {
    switch (wParam) {
    case 0x57: //'w' key
        m_linearVelocity.z = 5.0f;
        return;
    case 0x41: //'a' key
        m_linearVelocity.x = -5.0f;
        return;
    case 0x53: //'s' key
        m_linearVelocity.z = -5.0f;
        return;
    case 0x44: //'d' key
        m_linearVelocity.x = 5.0f;
        return;
    case VK_SPACE:
        m_linearVelocity.y = 5.0f;
        return;
    case VK_SHIFT:
        m_linearVelocity.y = -5.0f;
        return;
    }
}

void Player::OnKeyUp(WPARAM wParam) {
    switch (wParam) {
    case 0x57: //'w' key
        if (m_linearVelocity.z > 0)
            m_linearVelocity.z = 0.0f;
        return;
    case 0x41: //'a' key
        if (m_linearVelocity.x < 0)
            m_linearVelocity.x = 0.0f;
        return;
    case 0x53: //'s' key
        if (m_linearVelocity.z < 0)
            m_linearVelocity.z = 0.0f;
        return;
    case 0x44: //'d' key
        if (m_linearVelocity.x > 0)
            m_linearVelocity.x = 0.0f;
        return;
    case VK_SPACE:
        if (m_linearVelocity.y > 0)
            m_linearVelocity.y = 0.0f;
        return;
    case VK_SHIFT:
        if (m_linearVelocity.y < 0)
            m_linearVelocity.y = 0.0f;
        return;
    }
}

void Player::Move(float elapsedTime) {
    Camera::Move({ m_linearVelocity.x*elapsedTime, m_linearVelocity.y * elapsedTime, m_linearVelocity.z*elapsedTime });
}
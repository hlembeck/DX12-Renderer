#include "Game.h"

Game::Game(UINT width, UINT height) : Renderer(width,height) {}

Game::~Game() {}

void Game::OnInit() {
	Renderer::OnInit();
	Player::OnInit(45.0f, WindowInterface::GetAspectRatio(), 0.1f, 100.0f);
    Terrain::Load(m_commandQueue.Get());
    Lights::Load();
}

void Game::OnUpdate() {
    std::chrono::time_point<std::chrono::high_resolution_clock> curr = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> diff = curr - m_time;
    float elapsedTime = diff.count();
    m_time = curr;
    Player::Move(elapsedTime);
    if (m_inputCaptured) {
        POINT cursorPos = {};
        GetCursorPos(&cursorPos);
        Player::Pitch(((cursorPos.y - m_cursorPos.y) / m_mouseScaleFactor) * elapsedTime);
        Player::RotateWorldY(((cursorPos.x - m_cursorPos.x) / m_mouseScaleFactor) * elapsedTime);
        SetCursorPos(m_cursorPos.x, m_cursorPos.y);
    }
    Renderer::SetCameraConstants(Player::GetCameraConstants());
    Lights::OnUpdate(elapsedTime*0.05f);
    Terrain::Update(Player::GetPosition(), m_commandQueue.Get());
}

void Game::OnRender() {
    BasicRenderObject pObjects[] = { Terrain::GetRenderObject() };
    Renderer::OnRender(pObjects, 1, Lights::GetView());
}

void Game::OnKeyDown(WPARAM wParam) {
    if (wParam == VK_ESCAPE) {
        if (m_inputCaptured) {
            m_inputCaptured = FALSE;
            SetCursor(m_hCursor);
        }
        else {
            m_inputCaptured = TRUE;
            SetCursor(NULL);
        }
        return;
    }
    if (m_inputCaptured) {
        Player::OnKeyDown(wParam);
    }
}

void Game::OnKeyUp(WPARAM wParam) {
    if (m_inputCaptured) {
        Player::OnKeyUp(wParam);
    }
}

void Game::SetCursorPoint(POINT p, HCURSOR hCursor) {
	m_cursorPos = p;
	m_mouseScaleFactor = 20.0f;
	m_hCursor = hCursor;
	SetCursor(m_hCursor);
}

void Game::OnDestroy() {
	Renderer::OnDestroy();
}
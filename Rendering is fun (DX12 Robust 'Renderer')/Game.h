#pragma once
#include "Renderer.h"
#include "Player.h"
#include "Scene.h"

class Game :
	public Renderer,
	private Player,
	private Scene
{
public:
	Game(UINT width, UINT height);
	~Game();

	void OnInit() final;
	void OnUpdate() final;
	void OnRender() final;
	void OnDestroy() final;
	void OnKeyDown(WPARAM wParam) final;
	void OnKeyUp(WPARAM wParam) final;
	void SetCursorPoint(POINT p, HCURSOR hCursor) final;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_time;
	BOOL m_inputCaptured;
	POINT m_cursorPos;
	float m_mouseScaleFactor; //Sets angular velocity, will use fov to do this later
	HCURSOR m_hCursor;
};
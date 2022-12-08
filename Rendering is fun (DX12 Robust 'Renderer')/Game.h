#pragma once
#include "Renderer.h"
#include "Player.h"

class Game :
	private Renderer,
	private Player
{
public:
	Game(UINT width, UINT height);
	~Game();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_time;
	BOOL m_inputCaptured;
	POINT m_cursorPos;
	float m_mouseScaleFactor; //Sets angular velocity, will use fov to do this later
	HCURSOR m_hCursor;
};
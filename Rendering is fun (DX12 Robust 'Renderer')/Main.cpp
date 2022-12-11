#include "Main.h"

constexpr UINT initialWidth = 1920;
constexpr UINT initialHeight = 1080;

HWND Application::m_hWnd = 0;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    //Console for debugging
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);


    std::random_device rnd;
    std::mt19937 gen(rnd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);


    Game game(initialWidth, initialHeight);
    //Renderer renderer = Renderer(initialWidth, initialHeight);
    Application::Run(hInstance, nCmdShow, initialWidth, initialHeight, &game);
    return 0;
}
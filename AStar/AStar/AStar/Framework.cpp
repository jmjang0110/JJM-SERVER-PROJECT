#include "pch.h"
#include "Framework.h"
#include "Win32RenderMgr.h"
#include "GameMap.h"
#include "AStar.h"


Framework::Framework()
{
}

Framework::~Framework()
{
}

bool Framework::Init(HINSTANCE& hInstance, const LONG& width, const LONG& height)
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTAR));
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = L"A-star";
    wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);

    m_hWnd = CreateWindowW(L"A-star", L"A-star", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, width, height, nullptr, nullptr, m_hInst, nullptr);

    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hWnd);

    m_IsRun = 1;

    m_Resolution[0] = width;
    m_Resolution[1] = height;

    Win32RenderMgr::GetInstance()->Init(m_hWnd, width, height);
    m_Timer.Start();
    InitAStar();

    return true;
}

int Framework::Loop()
{
    static HACCEL hAccelTable = LoadAccelerators(m_hInst, MAKEINTRESOURCE(IDC_ASTAR));
    static MSG msg{};

    while (m_IsRun)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                continue;

            }
        }
        else
        {
            LoopLogic();
        }
    }

    ::DestroyWindow(m_hWnd);
    ::UnregisterClass(L"AStar", m_hInst);

    return (int)msg.wParam;
}

void Framework::LoopLogic()
{
    auto win32 = Win32RenderMgr::GetInstance();

    m_Timer.Update();
    float deltaTime = m_Timer.GetDeltaTime();

    // 화면 준비
    win32->Render_Prepare(m_Resolution[0], m_Resolution[1]);

    // 맵 그리기
    DrawGridMap();

    // deltaTime 텍스트
    wchar_t TimeText[50];
    swprintf_s(TimeText, L"Time: %.4f", m_Timer.GetAccumulatedTime());
    win32->DrawWText(POINT{10, 10}, TimeText);

    MoveNPC(deltaTime);

    // 렌더링 완료
    win32->Render_Present(m_Resolution[0], m_Resolution[1]);
}




void Framework::ChangeWindowSize(HWND _hwnd, const LONG& width, const LONG& height)
{
    RECT rt = { 0, 0, width, height };

    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);

    RECT rect_desktop = { 0, };
    HWND hWnd_desktop = GetDesktopWindow();
    GetWindowRect(hWnd_desktop, &rect_desktop);

    RECT rect_console = { 0, };
    HWND hWnd_console = GetConsoleWindow();
    GetWindowRect(hWnd_console, &rect_console);

    // 중앙에 화면 출력
    SetWindowPos(_hwnd
        , nullptr
        , (rect_desktop.left + rect_desktop.right - rect_console.right + rect_console.left) / 2 - (rt.right - rt.left) / 2
        , (rect_desktop.top + rect_desktop.bottom - rect_console.bottom + rect_console.top) / 2 - (rt.bottom - rt.top) / 2
        , rt.right - rt.left
        , rt.bottom - rt.top
        , 0);
}

LRESULT Framework::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) // Check if ESC key is pressed
        {
            DestroyWindow(hWnd);
            return 0;
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

INT_PTR Framework::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Framework::InitAStar()
{ 
    // 시작 위치와 종료 위치 설정
    int sy = 0, sx = 0;
    int ey = 19, ex = 19;

    // A* 객체 생성
    Object start(sy, sx);
    Object end(ey, ex);

    m_Astar.SetStart(start);
    m_Astar.SetEnd(end);

    static volatile long long time = 0;

    // 경로 탐색
    if (m_Astar.Update()) {
        std::cout << "경로를 찾았습니다.\n";

        // 경로를 찾았을 때, 단계별로 화면 갱신
        for (auto& node : m_Astar.GetPath()) {
            POSITION pos = node.GetPosition();
            GameMap[pos.y][pos.x] = '*'; // '*'로 경로 표시
        }
    }
    else {
        std::cout << "경로를 찾을 수 없습니다.\n";
    }
}

void Framework::DrawGridMap()
{
    // 맵 셀 크기
    int cellWidth = 40;  // 셀의 너비
    int cellHeight = 40; // 셀의 높이

    // 시작 지점 오프셋
    const int offsetX = 50; // X축 시작 위치
    const int offsetY = 50; // Y축 시작 위치

    // 맵을 그리기
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            POINT topLeft = { offsetX + x * cellWidth, offsetY + y * cellHeight }; // 셀의 좌상단 좌표

            // 셀 표시
            if (GameMap[y][x] == 1)
            {
                // 1일 경우 채워진 사각형 그리기 (벽 표시)
                Win32RenderMgr::GetInstance()->DrawFilledRectangle(topLeft, cellWidth, cellHeight, RGB(0, 0, 255));
            
            }
            else if (GameMap[y][x] == '*') 
            {
                Win32RenderMgr::GetInstance()->DrawFilledRectangle(topLeft, cellWidth, cellHeight, RGB(100, 250, 100)); 
            }
            else if (GameMap[y][x] == VISIT) {

                Win32RenderMgr::GetInstance()->DrawFilledRectangle(topLeft, cellWidth, cellHeight, RGB(204,255, 255)); 
            }
            else
            {
                // 0일 경우 빈 사각형 그리기 (바닥 표시)
                Win32RenderMgr::GetInstance()->DrawRectangle(topLeft, cellWidth, cellHeight);
            }

        }
    }
}

void Framework::MoveNPC(double deltaTime)
{

    // A* 경로를 따라 빨간색 원을 이동
    static size_t idx = 0; // 현재 경로 노드 인덱스

    static float x    = 75 + m_Astar.GetStart().GetPosition().x * 40;
    static float y    = 75 + m_Astar.GetStart().GetPosition().y * 40; // 빨간색 원의 시작 위치

    // 경로가 비어 있으면, A* 경로를 계산하여 저장
   
#define OPTIMAL_A_STAR_PATH
#ifdef OPTIMAL_A_STAR_PATH
    std::vector<Object> path = m_Astar.GetOptimalPath(); // A* 경로를 얻음
#else
    std::vector<Object> path = m_Astar.GetPath(); // A* 경로를 얻음
#endif

    if (idx == path.size()) {
        x = 75 + m_Astar.GetStart().GetPosition().x * 40;
        y = 75 + m_Astar.GetStart().GetPosition().y * 40;
        idx = 0;
    }

    // 경로를 따라 빨간색 원을 이동
    if (!path.empty() && idx < path.size()) {
        // 현재 위치와 목표 위치를 가져옴
        POSITION targetidx = path[idx].GetPosition();
        float target_x = 70 + targetidx.x * 40;
        float target_y = 70 + targetidx.y * 40;

        // 목표 지점으로 이동
        float speed = 10.f; // 이동 속도
        x = std::lerp(x, target_x, speed * deltaTime);
        y = std::lerp(y, target_y, speed * deltaTime);

        // 목표 지점에 도달하면 다음 노드로 이동
        if (std::abs(x - target_x) < 0.1f && std::abs(y - target_y) < 0.1f) {
            idx++;
        }
    }

    // 경로를 따라 빨간색 원 그리기
    Win32RenderMgr::GetInstance()->DrawPointColor(POINT{static_cast<int>(x), static_cast<int>(y)}, 5, RGB(255, 0, 0));

}

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

    // ȭ�� �غ�
    win32->Render_Prepare(m_Resolution[0], m_Resolution[1]);

    // �� �׸���
    DrawGridMap();

    // deltaTime �ؽ�Ʈ
    wchar_t TimeText[50];
    swprintf_s(TimeText, L"Time: %.4f", m_Timer.GetAccumulatedTime());
    win32->DrawWText(POINT{10, 10}, TimeText);

    MoveNPC(deltaTime);

    // ������ �Ϸ�
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

    // �߾ӿ� ȭ�� ���
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
        // �޴� ������ ���� �м��մϴ�:
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
        // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�...
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
    // ���� ��ġ�� ���� ��ġ ����
    int sy = 0, sx = 0;
    int ey = 19, ex = 19;

    // A* ��ü ����
    Object start(sy, sx);
    Object end(ey, ex);

    m_Astar.SetStart(start);
    m_Astar.SetEnd(end);

    static volatile long long time = 0;

    // ��� Ž��
    if (m_Astar.Update()) {
        std::cout << "��θ� ã�ҽ��ϴ�.\n";

        // ��θ� ã���� ��, �ܰ躰�� ȭ�� ����
        for (auto& node : m_Astar.GetPath()) {
            POSITION pos = node.GetPosition();
            GameMap[pos.y][pos.x] = '*'; // '*'�� ��� ǥ��
        }
    }
    else {
        std::cout << "��θ� ã�� �� �����ϴ�.\n";
    }
}

void Framework::DrawGridMap()
{
    // �� �� ũ��
    int cellWidth = 40;  // ���� �ʺ�
    int cellHeight = 40; // ���� ����

    // ���� ���� ������
    const int offsetX = 50; // X�� ���� ��ġ
    const int offsetY = 50; // Y�� ���� ��ġ

    // ���� �׸���
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            POINT topLeft = { offsetX + x * cellWidth, offsetY + y * cellHeight }; // ���� �»�� ��ǥ

            // �� ǥ��
            if (GameMap[y][x] == 1)
            {
                // 1�� ��� ä���� �簢�� �׸��� (�� ǥ��)
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
                // 0�� ��� �� �簢�� �׸��� (�ٴ� ǥ��)
                Win32RenderMgr::GetInstance()->DrawRectangle(topLeft, cellWidth, cellHeight);
            }

        }
    }
}

void Framework::MoveNPC(double deltaTime)
{

    // A* ��θ� ���� ������ ���� �̵�
    static size_t idx = 0; // ���� ��� ��� �ε���

    static float x    = 75 + m_Astar.GetStart().GetPosition().x * 40;
    static float y    = 75 + m_Astar.GetStart().GetPosition().y * 40; // ������ ���� ���� ��ġ

    // ��ΰ� ��� ������, A* ��θ� ����Ͽ� ����
   
#define OPTIMAL_A_STAR_PATH
#ifdef OPTIMAL_A_STAR_PATH
    std::vector<Object> path = m_Astar.GetOptimalPath(); // A* ��θ� ����
#else
    std::vector<Object> path = m_Astar.GetPath(); // A* ��θ� ����
#endif

    if (idx == path.size()) {
        x = 75 + m_Astar.GetStart().GetPosition().x * 40;
        y = 75 + m_Astar.GetStart().GetPosition().y * 40;
        idx = 0;
    }

    // ��θ� ���� ������ ���� �̵�
    if (!path.empty() && idx < path.size()) {
        // ���� ��ġ�� ��ǥ ��ġ�� ������
        POSITION targetidx = path[idx].GetPosition();
        float target_x = 70 + targetidx.x * 40;
        float target_y = 70 + targetidx.y * 40;

        // ��ǥ �������� �̵�
        float speed = 10.f; // �̵� �ӵ�
        x = std::lerp(x, target_x, speed * deltaTime);
        y = std::lerp(y, target_y, speed * deltaTime);

        // ��ǥ ������ �����ϸ� ���� ���� �̵�
        if (std::abs(x - target_x) < 0.1f && std::abs(y - target_y) < 0.1f) {
            idx++;
        }
    }

    // ��θ� ���� ������ �� �׸���
    Win32RenderMgr::GetInstance()->DrawPointColor(POINT{static_cast<int>(x), static_cast<int>(y)}, 5, RGB(255, 0, 0));

}

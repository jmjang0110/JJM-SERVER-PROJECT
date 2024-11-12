#include "pch.h"
#include "Framework.h"
#include "Win32RenderMgr.h"


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

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JJMSERVERSTRESSTEST));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"JJM Server Stress Test";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);

    m_hWnd = CreateWindowW(L"JJM Server Stress Test", L"JJM Server Stress Test", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, width, height, nullptr, nullptr, m_hInst, nullptr);

    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hWnd);

    m_IsRun = 1;

    m_Resolution[0] = width;
    m_Resolution[1] = height;
    Win32RenderMgr::GetInstance()->Init(m_hWnd, width, height);

    return true;
}

int Framework::Loop()
{
    static HACCEL hAccelTable = LoadAccelerators(m_hInst, MAKEINTRESOURCE(IDC_JJMSERVERSTRESSTEST));
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
    ::UnregisterClass(L"JJM_ServerStressTest", m_hInst);

    return (int)msg.wParam;
}

void Framework::LoopLogic()
{
    Win32RenderMgr::GetInstance()->Render_Prepare(m_Resolution[0], m_Resolution[1]);

    static int time = 0; time++;
    std::wstring timew = std::to_wstring(time);
    Win32RenderMgr::GetInstance()->DrawFilledRectangle(POINT { 10, 50 }, 50, 50, RGB(0, 0, 0)); // 내부 채우기
    Win32RenderMgr::GetInstance()->DrawWText(POINT{ 10, 0 }, L"Delay : " + timew, 30);
    Win32RenderMgr::GetInstance()->DrawPoint(POINT{ 30, 70 }, 4);

    Win32RenderMgr::GetInstance()->Render_Present(m_Resolution[0], m_Resolution[1]);

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

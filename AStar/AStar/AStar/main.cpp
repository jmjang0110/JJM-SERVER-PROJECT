// AStar.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "Framework.h"

Framework framework;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!framework.Init(hInstance, 1500, 1500)) {
        return -1;
    }

    int result = framework.Loop();
    return result;
}



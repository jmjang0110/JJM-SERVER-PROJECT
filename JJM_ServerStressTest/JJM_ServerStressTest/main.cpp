#include "pch.h"
#include "Framework.h"

int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
                      _In_opt_  HINSTANCE hPrevInstance,
                      _In_      LPWSTR    lpCmdLine,
                      _In_      int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!Framework::GetInstance()->Init(hInstance, 800, 600)) {  
        Framework::GetInstance()->Destroy();
        return -1; 
    }

    int result = Framework::GetInstance()->Loop();
    Framework::GetInstance()->Destroy();

    return result;
}


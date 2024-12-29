#include "pch.h"
#include "Framework.h"
#include "NetworkModule.h"

int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
                      _In_opt_  HINSTANCE hPrevInstance,
                      _In_      LPWSTR    lpCmdLine,
                      _In_      int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!Framework::GetInstance()->Init(hInstance, 1200, 600)) {  
        Framework::GetInstance()->Destroy();
        return -1; 
    }

    NetworkModule::GetInstance()->Init();
    NetworkModule::GetInstance()->Execute(6);
    int result = Framework::GetInstance()->Loop();

    Framework::GetInstance()->Destroy();
    NetworkModule::GetInstance()->Destroy();
    return result;
}


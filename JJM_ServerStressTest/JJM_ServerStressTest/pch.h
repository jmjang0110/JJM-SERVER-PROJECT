#pragma once


#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <winsock.h>
#include <windows.h>


#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <SDKDDKVer.h>


#include <iostream>
#include <vector>	
#include <map>
#include <list>
#include <unordered_map>
#include <queue>
#include <array>


#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

#include <DirectXMath.h>
#include "atlImage.h"
#include "resource.h"

// My Util Headers
#include "Singleton.h"

#pragma comment (lib, "ws2_32.lib")

typedef std::chrono::high_resolution_clock::time_point TimePoint;

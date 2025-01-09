#pragma once
#define WIN32_LEAN_AND_MEAN            

/********************************************/
// Default Headers
/********************************************/
#include "Resource.h"
#include <SDKDDKVer.h>
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


/********************************************/
// Headers
/********************************************/

#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <math.h>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <limits>

#include <DirectXMath.h>
#include "atlImage.h"


/********************************************/
// my Headers
/********************************************/

#include "Position.h"
#include "AStar.h"


/********************************************/
// utility class 
/********************************************/
template<typename T>
class Singleton
{
private:
	static T* m_Instance;

public:
	virtual ~Singleton() {};

public:
	static T* GetInstance() {
		if (m_Instance)
			return m_Instance;

		m_Instance = new T;
		return m_Instance;
	}

	static void Destroy() {
		if (m_Instance != nullptr) {
			delete m_Instance;
			m_Instance = nullptr;
		}
	}
};

template<typename T>
T* Singleton<T>::m_Instance = nullptr;



class Timer 
{
private:
    std::chrono::high_resolution_clock::time_point m_StartTime;  // 타이머 시작 시간
    std::chrono::high_resolution_clock::time_point m_PreviousTime; // 이전 프레임 시간
    std::chrono::high_resolution_clock::time_point m_CurrentTime;  // 현재 시간
    double m_DeltaTime;  // 델타 타임 (초 단위)
    double m_accumulated_time = 0;

public:
    Timer() : m_StartTime(), m_PreviousTime(), m_CurrentTime(), m_DeltaTime(0.0) {}

    // 타이머 시작
    void Start() {
        m_StartTime = std::chrono::high_resolution_clock::now();
        m_PreviousTime = m_StartTime;
        m_CurrentTime = m_StartTime;
    }

    // 델타타임 갱신
    void Update() {
        m_CurrentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = m_CurrentTime - m_PreviousTime;
        m_DeltaTime = elapsed.count(); // 초 단위의 델타타임
        m_PreviousTime = m_CurrentTime;
        
        m_accumulated_time += m_DeltaTime;
    }

    // 현재 델타타임 가져오기
    double GetDeltaTime() const {
        return m_DeltaTime;
    }

    double GetAccumulatedTime() const {
        return m_accumulated_time;
    }
    // 경과된 전체 시간
    double GetElapsedTime() const {
        std::chrono::duration<double> elapsed = m_CurrentTime - m_StartTime;
        return elapsed.count();
    }



};

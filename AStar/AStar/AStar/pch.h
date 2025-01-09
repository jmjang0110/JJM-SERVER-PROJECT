#pragma once
#define WIN32_LEAN_AND_MEAN            

/********************************************/
// Default Headers
/********************************************/
#include "Resource.h"
#include <SDKDDKVer.h>
// Windows ��� ����
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
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
    std::chrono::high_resolution_clock::time_point m_StartTime;  // Ÿ�̸� ���� �ð�
    std::chrono::high_resolution_clock::time_point m_PreviousTime; // ���� ������ �ð�
    std::chrono::high_resolution_clock::time_point m_CurrentTime;  // ���� �ð�
    double m_DeltaTime;  // ��Ÿ Ÿ�� (�� ����)
    double m_accumulated_time = 0;

public:
    Timer() : m_StartTime(), m_PreviousTime(), m_CurrentTime(), m_DeltaTime(0.0) {}

    // Ÿ�̸� ����
    void Start() {
        m_StartTime = std::chrono::high_resolution_clock::now();
        m_PreviousTime = m_StartTime;
        m_CurrentTime = m_StartTime;
    }

    // ��ŸŸ�� ����
    void Update() {
        m_CurrentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = m_CurrentTime - m_PreviousTime;
        m_DeltaTime = elapsed.count(); // �� ������ ��ŸŸ��
        m_PreviousTime = m_CurrentTime;
        
        m_accumulated_time += m_DeltaTime;
    }

    // ���� ��ŸŸ�� ��������
    double GetDeltaTime() const {
        return m_DeltaTime;
    }

    double GetAccumulatedTime() const {
        return m_accumulated_time;
    }
    // ����� ��ü �ð�
    double GetElapsedTime() const {
        std::chrono::duration<double> elapsed = m_CurrentTime - m_StartTime;
        return elapsed.count();
    }



};

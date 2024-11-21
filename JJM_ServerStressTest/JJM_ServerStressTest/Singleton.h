#pragma once

template<typename T>
class Singleton
{
private:
	static T* m_Instance;

public:
	virtual ~Singleton() { };

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
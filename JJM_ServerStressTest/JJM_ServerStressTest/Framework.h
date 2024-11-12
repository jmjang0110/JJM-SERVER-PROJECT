#pragma once


class Framework : public Singleton<Framework>
{
private:
	BYTE			m_IsRun{};
	LONG			m_Resolution[2]{}; // width / height
	HINSTANCE		m_hInst{};
	HWND			m_hWnd{};

public:
	Framework();
	virtual ~Framework();

public:
	bool Init(HINSTANCE& hInstance, const LONG& width, const LONG& height);
	int  Loop();
	void LoopLogic();

public:
	void ChangeWindowSize(HWND _hwnd, const LONG& width, const LONG& height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

};


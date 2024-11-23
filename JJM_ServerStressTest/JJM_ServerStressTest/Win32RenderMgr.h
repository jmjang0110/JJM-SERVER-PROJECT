#pragma once

constexpr int ROOM_WIDTH = 200;
constexpr int ROOM_HEIGHT = 200;

typedef DirectX::XMFLOAT2 vec2;
typedef DirectX::XMFLOAT3 vec3;
typedef DirectX::XMFLOAT4 vec4;

/****************
	  IMAGE
*****************/
struct Image
{
	ATL::CImage  image = {};
	vec2		 LT = {};
	vec2		 Size = {};
};

class Win32RenderMgr : public Singleton<Win32RenderMgr>
{
private:
	HWND		mhWnd{};

	HDC			mFrontHDC{};					// 최종 출력 HDC 
	HDC			mBackHDC{};						// 백버퍼 HDC 

	HBITMAP		mBackBuffer{};
	HBITMAP		mPrevBackBuffer{};

	HGDIOBJ		mPen{};
	HPEN		mPrevPen{};

	HBRUSH		mBrush{};  
public:
	Win32RenderMgr();
	~Win32RenderMgr();

	bool Init(HWND& hwnd, const LONG& wdith, const LONG& height);
	void Render_Prepare(const LONG& wdith, const LONG& height);
	void Render_Present(const LONG& wdith, const LONG& height);

	constexpr HDC& GetRenderHDC();

public:
	void DrawLine(const POINT& start, const POINT& end);
	void DrawPolygon(const POINT* apt, int cpt);
	void DrawRectangle(const POINT& topLeft, const int& width, const int& height);
	void DrawFilledRectangle(const POINT& topLeft, int width, int height, COLORREF fillColor);
	void DrawPoint(const POINT& position, int radius);
	void DrawImage(Image& drawImage, vec2& window_lt, vec2& window_size);
	void DrawWText(const POINT& position, const std::wstring& text);
	void DrawWText(const POINT& position, const std::wstring& text, int fontSize);

public:
	void SetPen(int style, int width, COLORREF color);
	void SetOriginPen();

};


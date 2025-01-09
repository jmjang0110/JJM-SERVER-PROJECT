#include "pch.h"
#include "Win32RenderMgr.h"
#include "Framework.h"

Win32RenderMgr::Win32RenderMgr()
{

}

Win32RenderMgr::~Win32RenderMgr()
{
	SelectObject(mFrontHDC, mBackHDC);
	ReleaseDC(mhWnd, mFrontHDC);
}

bool Win32RenderMgr::Init(HWND& hwnd, const LONG& wdith, const LONG& height)
{
	mhWnd	= hwnd;

	// Double buffering 
	mFrontHDC = GetDC(mhWnd);
	mBackHDC  = CreateCompatibleDC(mFrontHDC);

	mBackBuffer     = CreateCompatibleBitmap(mFrontHDC, wdith, height);
	mPrevBackBuffer = static_cast<HBITMAP>(SelectObject(mBackHDC, mBackBuffer));
	
	HFONT hFont = CreateFont(
		20,							 // Height of the font
		0,                           // Width of the font (0 means it will be calculated automatically)
		0,                           // Angle of the font's escapement
		0,                           // Angle of the font's orientation
		FW_NORMAL,                   // Font weight
		0,                           // Italic
		0,                           // Underline
		0,                           // Strikeout
		DEFAULT_CHARSET,             // Character set
		OUT_DEFAULT_PRECIS,          // Output precision
		CLIP_DEFAULT_PRECIS,         // Clipping precision
		DEFAULT_QUALITY,             // Output quality
		DEFAULT_PITCH | FF_DONTCARE, // Pitch and family
		L"Times New Roman"                     // Font name (change as needed)
	);
	
	return true;
}



void Win32RenderMgr::Render_Prepare(const LONG& wdith, const LONG& height)
{
    static RECT size = RECT{ 0,0, wdith, height };
	FillRect(mBackHDC, &size, (HBRUSH)(COLOR_WINDOW + 1));	// ����� ������� ĥ��
}


void Win32RenderMgr::Render_Present(const LONG& wdith, const LONG& height)
{
	// Back DC (BackBuffer RTV) �� �ִ� ȭ���� main HDC �� ���... < ���� ��� >
	BitBlt(mFrontHDC, 0, 0, wdith, height, mBackHDC, 0, 0, SRCCOPY);
}

constexpr HDC& Win32RenderMgr::GetRenderHDC()
{
	return mBackHDC;
}

void Win32RenderMgr::DrawLine(const POINT& start, const POINT& end)
{
	::MoveToEx(mBackHDC, start.x, start.y, NULL);
	::LineTo(mBackHDC, end.x, end.y);
}

void Win32RenderMgr::DrawPolygon(const POINT* apt, int cpt)
{
	::Polygon(mBackHDC, apt, cpt);
}

void Win32RenderMgr::DrawRectangle(const POINT& topLeft, const int& width, const int& height)
{
	::Rectangle(mBackHDC, topLeft.x, topLeft.y, topLeft.x + width, topLeft.y + height);
}

void Win32RenderMgr::DrawFilledRectangle(const POINT& topLeft, int width, int height, COLORREF fillColor)
{
	RECT rect = { topLeft.x, topLeft.y, topLeft.x + width, topLeft.y + height };
	HBRUSH hBrush = ::CreateSolidBrush(fillColor);
	::FillRect(mBackHDC, &rect, hBrush);

	::DeleteObject(hBrush);
}

void Win32RenderMgr::DrawPoint(const POINT& position, int radius)
{
	::Ellipse(mBackHDC, position.x - radius, position.y - radius, position.x + radius, position.y + radius);
}

void Win32RenderMgr::DrawPointColor(const POINT& position, int radius, COLORREF color)
{
	// ���� �׸��� ���� �귯�� ����
	HBRUSH hBrush = ::CreateSolidBrush(color);

	// ���� �׸��� ���� ���õ� �귯�ø� ����Ͽ� �׸���
	HGDIOBJ hPrevBrush = ::SelectObject(mBackHDC, hBrush);

	// �� �׸��� (ellipse �Լ� ���)
	::Ellipse(mBackHDC, position.x - radius, position.y - radius, position.x + radius, position.y + radius);

	// �귯�� ��ü�� ���� ������ �ǵ����� ����
	::SelectObject(mBackHDC, hPrevBrush);
	::DeleteObject(hBrush);
}


void Win32RenderMgr::DrawImage(Image& drawImage, vec2& window_lt, vec2& window_size)
{
	drawImage.image.Draw(mBackHDC
		, static_cast<int>(window_lt.x), static_cast<int>(window_lt.y)				// ������ â ȭ�鿡���� ��ġ
		, static_cast<int>(window_size.x), static_cast<int>(window_size.y)			// ������ â ȭ�鿡���� ������
		, static_cast<int>(drawImage.LT.x), static_cast<int>(drawImage.LT.y)		// �̹��������� ��ġ ( �ȼ� )
		, static_cast<int>(drawImage.Size.x), static_cast<int>(drawImage.Size.y));	// �̹��������� ������ ( �ȼ� )

}

void Win32RenderMgr::DrawWText(const POINT& position, const std::wstring& text)
{
	HFONT hPrevFont = (HFONT)SelectObject(mBackHDC, mFont);
	::TextOut(mBackHDC, position.x, position.y, text.c_str(), static_cast<int>(text.length()));
	SelectObject(mBackHDC, hPrevFont);
	DeleteObject(mFont);
}

void Win32RenderMgr::SetPen(int style, int width, COLORREF color)
{
	mPrevPen = ::CreatePen(style, width, color);
	mPen     = ::SelectObject(mBackHDC, mPrevPen);
}

void Win32RenderMgr::SetOriginPen()
{
	if (mPrevPen) {
		::DeleteObject(mPrevPen);
	}
	::SelectObject(mBackHDC, mPen);
}



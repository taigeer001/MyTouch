#include "Drawblock.h"

LPCWSTR szTitle = L"Drawblock";
LPCWSTR szWindowClass = L"Drawblock";

LRESULT CALLBACK DrawblockWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

BOOL HasDrawblockInit = FALSE;
BOOL DrawblocksInit()
{
    if (HasDrawblockInit == TRUE) {
        return TRUE;
    }
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_NOCLOSE;
    wcex.lpfnWndProc = DrawblockWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(0);
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = L"";
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = 0;

    Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken;
    GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

    RegisterClassExW(&wcex);
    
    HasDrawblockInit = TRUE;
    return HasDrawblockInit;
}


int Drawblocks::AppendRes(void* d)
{
    res.push_back(d);
    return (int)(res.size()-1);
}


int Drawblocks::LoadImage(const WCHAR* src)
{
    Image* pImage = Image::FromFile(src);
    
    Bitmap softwareBitmap(pImage->GetWidth(), pImage->GetHeight(), PixelFormat32bppARGB);
    Graphics g(&softwareBitmap);
    g.DrawImage(pImage, 0, 0);
    HBITMAP bmp;
    softwareBitmap.GetHBITMAP(Color(0, 0, 0, 0), &bmp);

    return AppendRes(bmp);
}

Drawblocks::Drawblocks()
{
    if (HasDrawblockInit == FALSE) {
        DrawblocksInit();
    }
}

Drawblock *Drawblocks::NewBlock()
{
    return NewBlock({ 0,0 }, { 200,200 });
}

Drawblock* Drawblocks::NewBlock(int w, int h)
{
    return NewBlock({ 0,0 }, { w,h });
}

Drawblock* Drawblocks::NewBlock(int x, int y, int w, int h)
{
    return NewBlock({ x,y }, { w,h });
}

Drawblock* Drawblocks::NewBlock(int x, int y, SIZE s)
{
    return NewBlock({ x, y }, s);
}

Drawblock* Drawblocks::NewBlock(POINT p, int w, int h)
{
    return NewBlock(p, { w,h });
}

template<typename T>
inline T Drawblocks::GetResource(int i)
{
    return (T)res[i];
}

Drawblock* Drawblocks::NewBlock(POINT p, SIZE s)
{
    HINSTANCE hInstance = GetModuleHandle(0);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        szWindowClass, szTitle, WS_VISIBLE | WS_POPUP,
        0, 0, 300, 300, GetDesktopWindow(), nullptr, hInstance, nullptr
    );

    if (!hwnd) {
        return NULL;
    }

    if (s.cx <= 0) {
        s.cx = 200;
    }
    if (s.cy <= 0) {
        s.cy = 200;
    }

    //ShowWindow(hwnd, SW_SHOW);
    //UpdateWindow(hwnd);

    return new Drawblock(this, hwnd, p, s);
}

Drawblocks::~Drawblocks()
{
    for (size_t i = 0; i < res.size(); i++)
    {
        delete(res[i]);
    }
}

Drawblock::Drawblock(Drawblocks* d, HWND h)
{
    Drawblock(d, h, POINT{ 0,0 }, SIZE{ 200,200 });
}

Drawblock::Drawblock(Drawblocks* d, HWND h, POINT p, SIZE s): parent(d), hwnd(h), loc(p), size(s)
{
    hdc = GetDC(hwnd);
    memdc = CreateCompatibleDC(hdc);
}

BOOL Drawblock::Refresh(int i)
{
    HBITMAP* bmp = parent->GetResource<HBITMAP*>(i);
    HGDIOBJ original = SelectObject(memdc, bmp);

    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    POINT ptSrc = { 0, 0 };
    BOOL l = UpdateLayeredWindow(hwnd, hdc, &loc, &size, memdc, &ptSrc, 0, &blend, ULW_ALPHA);
    int err = GetLastError();
    SelectObject(hdc, original);

    return TRUE;
}

Drawblock::~Drawblock()
{

}




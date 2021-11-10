#include <cmath>
#include <comdef.h>
#include "graphics.h"
#include "helper.h"

ID2D1Factory* pFactory;
IDWriteFactory* pWFactory;
void D2D1GraphicsInit() {
    if (pFactory && pWFactory) {
        return;
    }
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
        ErrorAndExit(L"D2d1 initialization error. [%d]", GetLastError());
    }
    if (FAILED(DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(pWFactory),
        reinterpret_cast<IUnknown**>(&pWFactory)
    ))) {
        ErrorAndExit(L"DWrite initialization error. [%d]", GetLastError());
    }
}

void D2D1Graphics::TextSize(const WCHAR* text, D2D1_SIZE_F& size) {
    IDWriteTextLayout* pTextLayout = NULL;
    if (FAILED(pWFactory->CreateTextLayout(text, (UINT32)wcslen(text), pFormat, 0.0f, 0.0f, &pTextLayout))) return;
    DWRITE_TEXT_METRICS textMetrics;
    if (SUCCEEDED(pTextLayout->GetMetrics(&textMetrics))) {
        size = D2D1::SizeF(ceil(textMetrics.widthIncludingTrailingWhitespace), ceil(textMetrics.height));
    }
    pTextLayout->Release();
}

D2D1Graphics::D2D1Graphics() {
    D2D1GraphicsInit();
    D2D1_RENDER_TARGET_PROPERTIES drtp = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_PREMULTIPLIED
        ));
    if (FAILED(pFactory->CreateDCRenderTarget(&drtp, &pRenderTarget))) {
        throw "CreateDCRenderTarget error.";
    }
    const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 1.0f);
    if (FAILED(pRenderTarget->CreateSolidColorBrush(color, &pBrush))) throw "CreateSolidColorBrush error.";
    if (FAILED(pWFactory->CreateTextFormat(L"Î¢ÈíÑÅºÚ", NULL, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fsize, L"", &pFormat))) {
        throw "CreateTextFormat error.";
    }
    pFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
}

ID2D1SolidColorBrush* D2D1Graphics::FontSize(FLOAT s) {
    return nullptr;
}

ID2D1SolidColorBrush* D2D1Graphics::Brush()
{
    return pBrush;
}

ID2D1SolidColorBrush* D2D1Graphics::Brush(Color)
{
    return nullptr;
}

ID2D1SolidColorBrush* D2D1Graphics::Brush(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
{
    if (a == -1.0f)   a = pBrush->GetOpacity();
    const D2D1_COLOR_F c = D2D1::ColorF(r, g, b, a);
    pBrush->SetColor(&c);
    return pBrush;
}

HRESULT D2D1Graphics::CreateBrush(ID2D1SolidColorBrush** pb, FLOAT r, FLOAT g, FLOAT b, FLOAT a)
{
    const D2D1_COLOR_F color = D2D1::ColorF(r, g, b, a);
    return pRenderTarget->CreateSolidColorBrush(color, pb);
}

ID2D1SolidColorBrush* D2D1Graphics::Brush(D2D1_COLOR_F)
{
    return nullptr;
}

void D2D1Graphics::BeginDraw(UINT16 w, UINT16 h, HWND hwnd)
{
    hdc = GetDC(hwnd);
    mdc = CreateCompatibleDC(hdc);
    hbitmap = CreateCompatibleBitmap(hdc, w, h);
    hobj = SelectObject(mdc, hbitmap);
    RECT rt{ 0,0,w,h };
    pRenderTarget->BindDC(mdc, &rt);
    pRenderTarget->BeginDraw();
}

void D2D1Graphics::BeginDraw(FLOAT w, FLOAT h)
{
    BeginDraw((UINT16)w, (UINT16)h);
}

void D2D1Graphics::BeginDraw()
{
    pRenderTarget->BeginDraw();
}

void D2D1Graphics::DrawRectangle(FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT s) {
    pRenderTarget->DrawRectangle(D2D1_RECT_F{ x + s / 2,y + s / 2,x + w - s / 2,y + h - s / 2 }, pBrush, s);
}

void D2D1Graphics::FillRectangle(FLOAT x, FLOAT y, FLOAT w, FLOAT h)
{
    pRenderTarget->FillRectangle(D2D1_RECT_F{ x,y,x + w,y + h }, pBrush);
}

void D2D1Graphics::FillRectangle(FLOAT x, FLOAT y, FLOAT w, FLOAT h, ID2D1SolidColorBrush* pb)
{
    pRenderTarget->FillRectangle(D2D1_RECT_F{ x,y,x + w,y + h }, pb);
}

void D2D1Graphics::FillEllipse(FLOAT x, FLOAT y, FLOAT w, FLOAT h)
{
    pRenderTarget->FillEllipse(D2D1_ELLIPSE{ x + (w / 2),y + (h / 2),w / 2,h / 2 }, pBrush);
}

void D2D1Graphics::DrawEllipse(FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT s)
{
    pRenderTarget->DrawEllipse(D2D1_ELLIPSE{ x + w / 2,y + h / 2, (w - s) / 2,(h - s) / 2 }, pBrush, s);
}

void D2D1Graphics::DrawTextW(const WCHAR* str, FLOAT x, FLOAT y, FLOAT w, FLOAT h)
{
    UINT32 len = (UINT32)wcslen(str);
    if (len == 0) return;
    D2D1_RECT_F drf{ x, y + ((h - fsize) / 2), x + w,y + ((h - fsize) / 2) + fsize };
    pRenderTarget->DrawTextW(str, len, pFormat, drf, pBrush);
}

void D2D1Graphics::DrawTextW(const WCHAR* str, FLOAT x, FLOAT y, FLOAT w, FLOAT h, ID2D1SolidColorBrush* pb) {
    UINT32 len = (UINT32)wcslen(str);
    if (len == 0) return;
    D2D1_RECT_F drf{ x, y + ((h - fsize) / 2), x + w,y + ((h - fsize) / 2) + fsize };
    pRenderTarget->DrawTextW(str, len, pFormat, drf, pb);
}

void D2D1Graphics::DrawText(const char* str, FLOAT x, FLOAT y, FLOAT w, FLOAT h) {
    wchar_t ws[100];
    swprintf(ws, 100, L"%hs", str);
}

void D2D1Graphics::DrawText(const char* str, FLOAT, FLOAT, FLOAT, FLOAT, ID2D1SolidColorBrush*) {

}

void D2D1Graphics::DrawLine(D2D1_POINT_2F p0, D2D1_POINT_2F p1, FLOAT s) {
    pRenderTarget->DrawLine(p0, p1, pBrush, s);
}

void D2D1Graphics::DrawLine(FLOAT x0, FLOAT y0, FLOAT x1, FLOAT y1, FLOAT s) {
    pRenderTarget->DrawLine(D2D1_POINT_2F{ x0,y0 }, D2D1_POINT_2F{ x1,y1 }, pBrush, s);
}

void D2D1Graphics::EndDraw()
{
    pRenderTarget->EndDraw();
    SelectObject(hdc, hobj);
}

void D2D1Graphics::Clear()
{
    DeleteObject(hdc);
    DeleteObject(mdc);
    DeleteObject(hbitmap);
}
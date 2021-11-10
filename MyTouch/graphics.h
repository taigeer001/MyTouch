#pragma once


#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include "type.h"

class D2D1Graphics :public IGraphics {
public:
	ID2D1DCRenderTarget* pRenderTarget = nullptr;
	IDWriteTextFormat* pFormat = nullptr;
	ID2D1SolidColorBrush* pBrush = nullptr;
	HDC hdc = 0, mdc = 0;
	HBITMAP hbitmap = nullptr;
	HGDIOBJ hobj = nullptr;
	FLOAT fsize = 14.0f;

	inline HDC GetCompatibleDC() {
		return mdc;
	}
	inline HDC GetHDC() {
		return hdc;
	}
	D2D1Graphics();
	ID2D1SolidColorBrush* FontSize(FLOAT);
	ID2D1SolidColorBrush* Brush();
	ID2D1SolidColorBrush* Brush(D2D1_COLOR_F);
	ID2D1SolidColorBrush* Brush(Color);
	ID2D1SolidColorBrush* Brush(FLOAT, FLOAT, FLOAT, FLOAT = -1.0f);
	HRESULT CreateBrush(ID2D1SolidColorBrush**, FLOAT, FLOAT, FLOAT, FLOAT = -1.0f);
	void TextSize(const WCHAR*, D2D1_SIZE_F&);
	forceinline virtual uint16 FontSize() override {
		return 14;
	};
	void BeginDraw();
	void BeginDraw(UINT16, UINT16, HWND = 0);
	void BeginDraw(FLOAT, FLOAT);
	void DrawRectangle(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT = 1.0f);
	void FillRectangle(FLOAT, FLOAT, FLOAT, FLOAT);
	inline void FillRectangle(FLOAT, FLOAT, FLOAT, FLOAT, ID2D1SolidColorBrush*);
	void FillEllipse(FLOAT, FLOAT, FLOAT, FLOAT);
	void DrawEllipse(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT = 1.0f);
	void DrawTextW(const WCHAR*, FLOAT, FLOAT, FLOAT, FLOAT);
	void DrawTextW(const WCHAR*, FLOAT, FLOAT, FLOAT, FLOAT, ID2D1SolidColorBrush*);
	void DrawText(const char*, FLOAT, FLOAT, FLOAT, FLOAT);
	void DrawText(const char*, FLOAT, FLOAT, FLOAT, FLOAT, ID2D1SolidColorBrush*);
	void DrawLine(D2D1_POINT_2F, D2D1_POINT_2F, FLOAT);
	void DrawLine(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT);
	void EndDraw();
	void Clear();
};


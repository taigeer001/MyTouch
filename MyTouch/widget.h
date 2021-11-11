#pragma once
#pragma warning(disable: 4250)

#include <vector>
#include "win.h"
#include "graphics.h"
#include "type.h"
#include "handler.h"
#include <iostream>

class Factory :public IHandler {
protected:
	IGraphics* ddg = nullptr;
	ICursor* cursor = nullptr;
	IInput* input = nullptr;
	std::vector<IWidget*> widget;
	float32 speed = 1.8f;
public:
	forceinline void Graphics(IGraphics* g) {
		ddg = g;
	}
	forceinline void Cursor(ICursor* c) {
		cursor = c;
	}
	forceinline void Input(IInput* i) {
		input = i;
	}
	forceinline void Speed(float32 s) {
		speed = s;
	}
	forceinline IGraphics* Graphics() override {
		return ddg;
	}
	forceinline void Widget(IWidget* w) override {
		widget.push_back(w);
		w->Handler(this);
		w->Initialization();
		w->OnPaint();
	}
	void Default();
	void OnTimer() override;
	void OnPoint(TouchPoint&) override;
	virtual void Start() {
		input->Start();
	}
};

class Widget : public IWidget {
protected:
	IHandler* handler = nullptr;
	HWND hwnd = 0;
public:
	Widget();
	~Widget();
	forceinline virtual IHandler* Handler() override {
		return handler;
	}
	forceinline virtual void Handler(IHandler* hl) override {
		handler = hl;
	}
	using DpiRectangle::Point;
	inline void Point(FLOAT x, FLOAT y) {
		DpiRectangle::Point(x, y);
		if (handler) OnPaint();
	}
	void Initialization() override;
	void Show(byte) override;
	void OnPaint() override {}
	virtual void Refresh() {}
protected:
	static DWORD WINAPI Msgloop(LPVOID);
	void UpdateLayered();
};

class Hotspot : public IHotspot {
protected:
	float32 rx = 0, ry = 0;
	// BYTE status = 0;
	Widget* parent = nullptr;
public:
	inline FLOAT Rx() {
		return rx;
	}
	inline void Rx(FLOAT v) {
		rx = v;
	}
	inline FLOAT Ry() {
		return ry;
	}
	inline void Ry(FLOAT v) {
		ry = v;
	}
	inline Widget* Parent() {
		return parent;
	}
	inline void Parent(Widget* v) {
		parent = v;
	}
	Hotspot(UINT16 w, UINT16 h) { Size(w, h); };
	//virtual void OnDown(InjectPoint*) override;
	//virtual void OnUp(InjectPoint*)  override;
	virtual void OnPaint(IGraphics*) {};
	virtual void Initialization() {};
};

class HotspotGroup : public Widget, protected IHotspot {
protected:
	std::vector<Hotspot*> button;
public:
	using Widget::X;
	using Widget::Y;
	virtual void Initialization() override;
	virtual void Point(FLOAT, FLOAT) override;
	virtual void X(FLOAT) override;
	virtual void Y(FLOAT) override;
	void Scale(FLOAT);
	void Hotspot(INT16, INT16, Hotspot*);
	virtual void OnPaint() override;
};

class WidgetHotspot : public Widget, protected IHotspot {
protected:
	int32 status = 0;
	const WCHAR* name;
public:
	WidgetHotspot();
	WidgetHotspot(const char*);
	~WidgetHotspot();
	void Initialization() override;
	void OnPaint() override;
	forceinline void OnDown(InjectPoint*) override {
		status = 1;
		OnPaint();
	};
	forceinline void OnUp(InjectPoint*) override {
		status = 0;
		OnPaint();
	};
};
#pragma once

#include <Windows.h>
#include <vector>
#include <iostream>
#include "helper.h"
#include <d2d1.h>
#include <dwrite.h>

class IInject;
class IWidget;
class IHotspot;
class IHandler;
class InjectData;

extern InjectData& InjectDataNull;

enum class InjectType {
	None = 0,
	LeftDown = 0x1,
	LeftUp = 0x2,
	RightDown = 0x4,
	RightUp = 0x8,
	Move = 0x10,
	To = 0x20,
	KeyDown = 0x40,
	KeyUp = 0x80,
	KeyHold = 0x81,
	KeyMove = 0x82,
};

enum class Color {
};

class StackId {
protected:
	UINT16 hasbit = 0;
public:
	forceinline UINT16 Id() {
		if (!(hasbit & 0x1)) return 0;
		if (!Getbit(hasbit, 1)) return 1;
		if (!Getbit(hasbit, 2)) return 2;
		if (!Getbit(hasbit, 3)) return 3;
		if (!Getbit(hasbit, 4)) return 4;
		if (!Getbit(hasbit, 5)) return 5;
		if (!Getbit(hasbit, 6)) return 6;
		if (!Getbit(hasbit, 7)) return 7;
		if (!Getbit(hasbit, 8)) return 8;
		if (!Getbit(hasbit, 9)) return 9;
		return 10;
	}
	forceinline void Occupy(UINT16 id) {
		hasbit = Setbit(hasbit, id);
	}
	forceinline void Release(UINT16 id) {
		hasbit = Clearbit(hasbit, id);
	}
	forceinline void Clear() {
		hasbit = 0;
	}
};

class DpiRectangle {
private:
	struct _Config {
		BYTE left : 2;
		BYTE right : 2;
		BYTE top : 2;
		BYTE bottom : 2;
		_Config() :left(1), right(0), top(1), bottom(1) {};
	};
	FLOAT x = 0, y = 0, width = 0, height = 0;
public:
	virtual forceinline FLOAT X() {
		return x;
	}
	virtual forceinline void X(FLOAT v) {
		x = v;
	}
	virtual forceinline FLOAT Y() {
		return y;
	}
	virtual forceinline void Y(FLOAT v) {
		y = v;
	}
	virtual forceinline FLOAT Width() {
		return width;
	}
	virtual forceinline void Width(FLOAT v) {
		width = v;
	}
	virtual forceinline FLOAT Height() {
		return height;
	}
	virtual forceinline void Height(FLOAT v) {
		height = v;
	}
	virtual forceinline void Point(FLOAT vx, FLOAT vy) {
		x = vx, y = vy;
	}
	virtual forceinline void Size(FLOAT w, FLOAT h) {
		width = w, height = h;
	}
	virtual forceinline POINT Point() {
		return POINT{ (LONG)x,(LONG)y };
	}
	virtual forceinline SIZE Size() {
		return SIZE{ (LONG)width, (LONG)height };
	}
	virtual forceinline FLOAT Left() {
		return x;
	}
	virtual forceinline FLOAT Right() {
		return x + width;
	}
	virtual forceinline FLOAT Top() {
		return y;
	}
	virtual forceinline FLOAT Bottom() {
		return y + height;
	}
	forceinline BYTE Inside(FLOAT px, FLOAT py) {
		return ((px >= Left() && px <= Right()) && (py >= Top() && py <= Bottom()));
	}
	_Config Config;
	virtual void OnDisplayChange(UINT, UINT, UINT, UINT);
};

class InjectPoint {
public:
	INT16 x = 0;
	INT16 y = 0;
	INT16 ax = 0;
	INT16 ay = 0;
};

class TouchPoint {
public:
	BYTE sta = 0;
	BYTE first = 0;
	UINT16 id = 0;
	INT16 x = 0;
	INT16 y = 0;
	UINT64 time = 0;
	TouchPoint& operator=(MSG&);
	forceinline void CloneTo(TouchPoint* tp) {
		memcpy(tp, this, sizeof(TouchPoint));
	}
};

class PointInfo {
public:
	BYTE ok : 1;
	BYTE id : 7;
	IInject* ijb = nullptr;
	PointInfo() :ok(0), id(0), ijb(nullptr) {};
};

class PointTable : public StackId {
public:
	PointInfo* info = nullptr;
	PointInfo** idt = nullptr;
	PointTable() {
		Clear();
		info = new PointInfo[0xFFFF];
		idt = new PointInfo * [10];
	}
	~PointTable() {
		delete[] info, idt;
	}
};

class InjectData {
private:
	struct Data {
		InjectType Type = InjectType::None;
		InjectPoint Point;
		IHotspot* Hot = nullptr;
	};
	size_t size = 0;
	size_t length = 0;
	Data* value = nullptr;
public:
	~InjectData() {
		Clear();
	}
	inline Data& operator[](size_t i) {
		return value[i];
	}
	inline void Size(size_t s) {
		if (s == size) return;
		Clear();
		size = s, value = new InjectData::Data[s];
	}
	inline size_t Size() {
		return size;
	}
	inline void Clear() {
		SafeDeletes(value);
	}
	inline size_t Length() {
		return length;
	}
	inline void Length(size_t l) {
		length = l;
	}
	inline void Append(InjectType type, INT16 x, INT16 y) {
		value[length].Type = type;
		value[length].Point.x = x;
		value[length++].Point.y = y;
	}
	inline void Append(InjectType type, TouchPoint* tp) {
		value[length].Type = type;
		value[length].Point.x = tp->x;
		value[length++].Point.y = tp->y;
	}
	inline void Append(InjectType type, TouchPoint* tp, INT16 ax, INT16 ay) {
		value[length].Type = type;
		value[length].Point.x = tp->x;
		value[length].Point.y = tp->y;
		value[length].Point.ax = ax;
		value[length++].Point.ay = ay;
	}
	inline void Append(InjectType type, IHotspot* hp) {
		value[length].Type = type;
		value[length++].Hot = hp;
	}
	inline void Append(InjectType type, IHotspot* hp, TouchPoint* tp) {
		value[length].Type = type;
		value[length].Hot = hp;
		value[length].Point.x = tp->x;
		value[length++].Point.y = tp->y;
	}
	inline void Append(InjectType type, IHotspot* hp, TouchPoint* tp, INT16 ax, INT16 ay) {
		value[length].Type = type;
		value[length].Hot = hp;
		value[length].Point.x = tp->x;
		value[length].Point.y = tp->y;
		value[length].Point.ax = ax;
		value[length++].Point.ay = ay;
	}
};

class IHotspot :virtual public DpiRectangle {
private:
	UINT16 id = 0;
	BYTE enable = 1;
public:
	forceinline UINT16 Id() {
		return id;
	}
	forceinline void Id(UINT16 v) {
		id = v;
	}
	forceinline BYTE Enable() {
		return enable;
	}
	forceinline void Enable(BYTE v) {
		enable = v;
	}
	virtual void OnDown(InjectPoint*) {};
	virtual void OnHold(InjectPoint*) {};
	virtual void OnUp(InjectPoint*) {};
	virtual void OnMove(InjectPoint*) {};
};

class ICursor {
public:
	virtual void Move(INT16, INT16) = 0;
	virtual void Pos(INT16, INT16) = 0;
	virtual void Initialization() = 0;
};

class IInputCallback {
public:
	virtual void OnInit() {};
	virtual void OnDisplayChange() {};
	virtual void OnTimer() {};
	virtual void OnPoint(TouchPoint&) {};
};

class IInput {
public:
	virtual IInputCallback* Callback() = 0;
	virtual void Callback(IInputCallback* ihd) = 0;
	virtual void Initialization() = 0;
	virtual void Start() = 0;
};

class IGraphics {
public:
	virtual HDC GetCompatibleDC() = 0;
	virtual HDC GetHDC() = 0;
	virtual ID2D1SolidColorBrush* FontSize(FLOAT) = 0;
	virtual ID2D1SolidColorBrush* Brush() = 0;
	virtual ID2D1SolidColorBrush* Brush(D2D1_COLOR_F) = 0;
	virtual ID2D1SolidColorBrush* Brush(Color) = 0;
	virtual ID2D1SolidColorBrush* Brush(FLOAT, FLOAT, FLOAT, FLOAT = -1.0f) = 0;
	virtual HRESULT CreateBrush(ID2D1SolidColorBrush**, FLOAT, FLOAT, FLOAT, FLOAT = -1.0f) = 0;
	virtual void TextSize(const WCHAR*, D2D1_SIZE_F&) = 0;
	virtual uint16 FontSize() = 0;
	virtual void BeginDraw() = 0;
	virtual void BeginDraw(UINT16, UINT16, HWND = 0) = 0;
	virtual void BeginDraw(FLOAT, FLOAT) = 0;
	virtual void DrawRectangle(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT = 1.0f) = 0;
	virtual void FillRectangle(FLOAT, FLOAT, FLOAT, FLOAT) = 0;
	virtual void FillRectangle(FLOAT, FLOAT, FLOAT, FLOAT, ID2D1SolidColorBrush*) = 0;
	virtual void FillEllipse(FLOAT, FLOAT, FLOAT, FLOAT) = 0;
	virtual void DrawEllipse(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT = 1.0f) = 0;
	virtual void DrawTextW(const WCHAR*, FLOAT, FLOAT, FLOAT, FLOAT) = 0;
	virtual void DrawTextW(const WCHAR*, FLOAT, FLOAT, FLOAT, FLOAT, ID2D1SolidColorBrush*) = 0;
	virtual void DrawText(const char*, FLOAT, FLOAT, FLOAT, FLOAT) = 0;
	virtual void DrawText(const char*, FLOAT, FLOAT, FLOAT, FLOAT, ID2D1SolidColorBrush*) = 0;
	virtual void DrawLine(D2D1_POINT_2F, D2D1_POINT_2F, FLOAT) = 0;
	virtual void DrawLine(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT) = 0;
	virtual void EndDraw() = 0;
	virtual void Clear() = 0;
};

class IInject {
protected:
	size_t length = 0;
	IHotspot** hotspot = nullptr;
public:
	virtual const char* TypeName() { return ""; };
	virtual void Initialization() {};
	virtual void Refresh() {};
	virtual InjectData* StartInput(PointTable*, PointInfo*, TouchPoint*) { return &InjectDataNull; };
	virtual InjectData* Input(PointTable*, PointInfo*, TouchPoint*) { return &InjectDataNull; };
	virtual InjectData* CloseInput(PointTable*, PointInfo*, TouchPoint*) { return &InjectDataNull; };
	virtual void OnPointChange() {};
	virtual void OnDisplayChange() {};
	virtual void Hotspot(IHotspot*);
};

class IHandler : public IInputCallback {
protected:
	PointTable table;
	std::vector<IInject*> obj;
	InjectData* OpenInject(PointInfo*, TouchPoint*);
public:
	virtual void OnPointChange() {};
	virtual IGraphics* Graphics() = 0;
	virtual void Cursor(ICursor*) = 0;
	virtual void Widget(IWidget*) = 0;
	virtual void Inject(IInject*);
	virtual InjectData& Behavior(TouchPoint*);
	virtual void Hotspot(const char*, IHotspot*);
};

class IWidget :virtual public DpiRectangle {
public:
	virtual IHandler* Handler() = 0;
	virtual void Handler(IHandler*) = 0;
	virtual void Initialization() = 0;
	virtual void OnPaint() = 0;
	virtual void Show(byte) = 0;
};

template<typename T>
class IVariate {
public:
	virtual T* Ptr() = 0;
	virtual void Ptr(T*) = 0;
};

template<typename T>
class Variate :IVariate<T> {
protected:
	T value = 0;
	T* ptr = &value;
public:
	T* Ptr() override {
		return ptr;
	}
	void Ptr(T* p) override {
		ptr = p;
	}
};

typedef Variate<int32> VariateInt;
typedef Variate<byte> VariateByte;

/*template<typename T>
class IVariate {
public:
	virtual IWidget* Widget() {
		return nullptr;
	};
	virtual T* Ptr() {
		return nullptr;
	};
	virtual void BindGet(IVariate<T>*) {};
	virtual void BindSet(IVariate<T>*) {};
	virtual void Bind(IVariate<T>* iv) {
		BindGet(iv);
		iv->BindSet(this);
	}
	virtual void OnChange(T*) {};
};

typedef IVariate<int32> IVariateInt;*/
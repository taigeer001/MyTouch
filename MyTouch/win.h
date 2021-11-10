#pragma once

#include <Windows.h>
#include <mutex>
#include "helper.h"
#include "type.h"

HWND DrawingWindow();

HWND MsgWindow();

class InputAndCursor : public ICursor, public IInput {
protected:
	HDC hdc;
	HWND hwnd = 0;
	HCURSOR cursor;
	POINT psrc = { 0,0 };
	SIZE ps = { 32, 32 };
	BLENDFUNCTION blend = { 0 };
	IInputCallback* callback = nullptr;
public:
	inline IInputCallback* Callback() {
		return callback;
	}
	inline void Callback(IInputCallback* icb) {
		if (!icb) return;
		callback = icb;
	}
	~InputAndCursor();
	virtual void Start() override;
	virtual void Move(INT16, INT16) override;
	virtual void Pos(INT16, INT16) override;
	virtual void Initialization() override;
};

class DebugInput : public InputAndCursor {
public:
	virtual void Start() override;
};

class DebugDev : public InputAndCursor {
public:
	virtual void Start() override;
};
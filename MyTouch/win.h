#pragma once

#include <Windows.h>
#include <mutex>
#include "helper.h"
#include "type.h"

HWND DefaultWindow();

HWND DrawingWindow();

HWND MsgWindow();

class InputAndCursor : public ICursor, public IInput {
protected:
	HDC hdc;
	HWND hwnd = 0;
	HCURSOR cursor = nullptr;
	int px = 0, py = 0;
	POINT psrc = { 0,0 };
	SIZE ps = { 32, 32 };
	BLENDFUNCTION blend = { 0 };
	IInputCallback* callback = nullptr;
	byte ismove = 1;
	static DWORD WINAPI CursorChange(LPVOID);
public:
	forceinline IInputCallback* Callback() {
		return callback;
	}
	forceinline void Callback(IInputCallback* icb) {
		if (!icb) return;
		callback = icb;
	}
	~InputAndCursor();
	void Start() override;
	forceinline byte CursorChange(CURSORINFO* ci) {
#ifndef _WINDOWS
		//std::cout << cursor << std::endl;
#endif // !_WINDOWS
		ci->ptScreenPos.x = ci->ptScreenPos.x - px;
		ci->ptScreenPos.y = ci->ptScreenPos.y - py;
		if (ci->hCursor && ci->hCursor != cursor) {
			cursor = ci->hCursor;
			switch ((uint)cursor) {
			case 0x10005: px = py = 10;break;
			case 0x1001b:
			case 0x10003: px = py = 0;break;
			case 0x10009:
			case 0x10007: px = py = 16;break;
			case 0x10011: px = 11, py = 5;break;
			case 0x10013: px = 5, py = 11;break;
			case 0x10015: px = py = 12;break;
			case 0x10017: px = py = 8;break;
			case 0x10019: px = 0, py = 5;break;
			case 0x1000B: px = 5, py = 0;break;
			case 0x1000D: px = py = 9;break;
			case 0x1000F: px = py = 9;break;
			case 0x1001F: px = 6, py = 0;break;
			case 0x20607: px = 20, py = 0;break;
			default:px = py = 0;
			}
			return 1;
		}
		return 0;
	}
	void Move(INT16, INT16) override;
	void Pos(INT16, INT16) override;
	void Initialization() override;
};

class DebugInput : public InputAndCursor {
public:
	void Start() override;
};

class DebugDev : public InputAndCursor {
public:
	void Start() override;
};
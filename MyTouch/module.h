#pragma once

#include <functional>
#include <map>
#include <vector>
#include "widget.h"
#include "type.h"
#include "parser.h"

class WidgetBox : public IWidget, public IMergeNode {
protected:
	IHandler* parent = nullptr;
	size_t length = 0;
	IWidget** widget = nullptr;
public:
	IHandler* Handler() override {
		return parent;
	}
	void Handler(IHandler* h) override {
		parent = h;
	}
	void Initialization() override {}
	void OnPaint() override {}
	void Show(byte s) override {
		for (size_t i = 0; i < length; i++) {
			widget[i]->Show(s);
		}
	}
	void Widget(IWidget* wg) {
		for (size_t i = 0; i < length; i++) {
			if (widget[i] == wg) return;
		}
		widget = Extension<IWidget>(widget, length);
		widget[length++] = wg;
	}
};

class SelectBox : public IWidget, public IMergeNode {
protected:
	IHandler* parent = nullptr;
	size_t wid = 0;
	size_t length = 0;
	IWidget** widget = nullptr;
public:
	IHandler* Handler() override {
		return parent;
	}
	void Handler(IHandler* h) override {
		parent = h;
	}
	void Initialization() override {}
	void OnPaint() override {}
	void Show(byte s) override {
		if (!s) {
			for (size_t i = 0; i < length; i++) {
				widget[i]->Show(0);
			}
			return;
		}
		widget[wid]->Show(1);
#ifndef _WINDOWS
		std::cout << "show: " << (int)s << std::endl;
#endif // !_WINDOWS

	}
	void Widget(IWidget* wg) {
		for (size_t i = 0; i < length; i++) {
			if (widget[i] == wg) return;
		}
		widget = Extension<IWidget>(widget, length);
		widget[length++] = wg;
	}
	void Default(size_t id) {
		if (id >= length) id = 0;
		int32 s = 0;
		wid = id;
		OnMerge(0, &s);
	}
	void OnMerge(uint16, void* o) override {
		int32 v = *(int32*)o;
#ifndef _WINDOWS
		std::cout << "OnMerge: " << length << std::endl;
#endif // !_WINDOWS
		if (v == 1) return;
		if (++wid == length) wid = 0;
		for (size_t i = 0; i < length; i++) {
			widget[i]->Show(i == wid);
		}
	}
};

class IKey :public Hotspot, public IMergeNode {
protected:
	const WCHAR* name;
	int32 status_value = 0;
	int32* status = &status_value;
public:
	IKey(uint16 w, uint16 h, const char* n = "") :Hotspot(w, h) {
		name = CharToWchar(n);
	}
	int32* Ptr() override {
		return status;
	}
	void Ptr(int32* v) override {
		if (!v) return;
		status = v;
	}
	void Initialization() override;
	void OnPaint(IGraphics*) override;
};

class BinKey {
protected:
	byte* downbin = nullptr;
	byte* upbin = nullptr;
	void Runbin(byte*);
public:
	~BinKey();
	void DownCode(byte*);
	void UpCode(byte*);
};

class FnKey :public IKey, public BinKey {
public:
	using IKey::IKey;
	~FnKey();
	void OnDown(InjectPoint*) override;
	void OnUp(InjectPoint*)  override;
};

class Key :public IKey {
protected:
	byte code = 0;
	uint16 redown = 0;
public:
	using IKey::IKey;
	void Code(byte);
	void OnDown(InjectPoint*) override;
	void OnHold(InjectPoint*) override;
	void OnUp(InjectPoint*)  override;
};

class MouseKey :public IKey {
protected:
	uint16 dcode = 0, ucode = 0;
public:
	using IKey::IKey;
	void Code(uint16, uint16);
	virtual void OnDown(InjectPoint*) override;
	virtual void OnUp(InjectPoint*)  override;
};

class Wheel :public IKey {
protected:
	int16 px = 0, py = 0;
	float32 speed = 2.0f;
public:
	using IKey::IKey;
	forceinline void Speed(float32 s) {
		speed = s;
	};
	void OnDown(InjectPoint*) override;
	void OnUp(InjectPoint*)  override;
	void OnMove(InjectPoint*) override;
};

class Letter :public Key {
protected:
	const WCHAR* name1;
	int32 t = 0;
	int32* pt = &t;
public:
	Letter(uint16 w, uint16 h, const char* n0 = "", const char* n1 = "") :Key(w, h, n0) {
		name1 = CharToWchar(n1);
	}
	void Ptr(int32* p) override {
		pt = p;
	}
	void Initialization() override;
	void OnPaint(IGraphics*) override;
};

class Symbol :public Letter {
public:
	using Letter::Letter;
	void OnPaint(IGraphics*) override;
};

class Capskey :public IKey {
protected:
	byte code = 0;
public:
	using IKey::IKey;
	void Code(byte);
	forceinline void OnDown(InjectPoint*) override {
		keybd_event(code, 0, 0, 0);
		keybd_event(code, 0, KEYEVENTF_KEYUP, 0);
		*status = !!GetKeyState(code);
		if (HasMerge()) CallMerge(status); else parent->OnPaint();
	}
};

class Keypad :public HotspotGroup, public IMergeNode {
public:
	forceinline void OnMerge(uint16, void*) override {
		OnPaint();
	}
	void Show(byte v) override {
		HotspotGroup::Show(v);
		size_t len = button.size();
		::Hotspot** d = button.data();
		for (size_t i = 0; i < len; i++) {
			d[i]->Enable(v);
		}
	}
};

class Button :public WidgetHotspot, public IMergeNode, public BinKey {
public:
	int32* Ptr() override {
		return &status;
	}
	void Ptr(int32*) override {

	}
	void OnMerge(uint16, void*) override {
		OnPaint();
	}
	forceinline void OnDown(InjectPoint*) override {
		status = 1;
		if (HasMerge()) CallMerge(&status); else OnPaint();
		if (downbin) Runbin(downbin);
	}
	forceinline void OnUp(InjectPoint*) override {
		status = 0;
		if (HasMerge()) CallMerge(&status); else OnPaint();
		if (upbin) Runbin(upbin);
	}
	using WidgetHotspot::WidgetHotspot;
	~Button();
	void Scale(FLOAT);
};

#define InitTemplate(c) \
void Init(pugi::xml_node& node, void* o, IDocumentParser* dp) override {\
	##c##* t = (##c##*)o;\
	if (node.attribute("name").empty()) return;\
	IMergeNode* mn = dp->GetMerge(node.attribute("name").as_string());\
	t->Ptr(mn->Ptr());\
	mn->Merge(t, 0);\
	mn->Merge((IMergeNode*)(Keypad*)t->Parent());\
}
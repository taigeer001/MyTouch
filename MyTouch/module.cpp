#include "module.h"
#include "helper.h"

void IKey::Initialization() {
	parent->Handler()->Hotspot("key", this);
}

void IKey::OnPaint(IGraphics* g) {
	if (*status) {
		g->Brush(1.0f, 1.0f, 1.0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(.0f, .0f, .0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(1.0f, 1.0f, 1.0f, .85f);
		g->DrawTextW(name, Rx(), Ry(), Width(), Height());
	}
	else {
		g->Brush(.0f, .0f, .0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(1.0f, 1.0f, 1.0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(.0f, .0f, .0f, .85f);
		g->DrawTextW(name, Rx(), Ry(), Width(), Height());
	}
}

void FnKey::OnDown(InjectPoint*) {
	*status = 1;
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
}

void FnKey::OnUp(InjectPoint*) {
	*status = 0;
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
}

void Key::Code(byte c) {
	code = c;
}

void Key::OnDown(InjectPoint*) {
	*status = 1, redown = 0;
	keybd_event(code, 0, 0, 0);
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
}

void Key::OnHold(InjectPoint*) {
	if (redown == 5) {
		keybd_event(code, 0, 0, 0);
		return;
	}
	redown++;
}

void Key::OnUp(InjectPoint*) {
	*status = 0;
	keybd_event(code, 0, KEYEVENTF_KEYUP, 0);
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
}

void MouseKey::Code(uint16 d, uint16 u) {
	dcode = d, ucode = u;
}

void MouseKey::OnDown(InjectPoint*) {
	*status = 1;
	mouse_event(dcode, 0, 0, 0, 0);
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
}

void MouseKey::OnUp(InjectPoint*) {
	*status = 0;
	mouse_event(ucode, 0, 0, 0, 0);
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
}

void Wheel::OnDown(InjectPoint* ijp) {
	*status = 1;
	px = ijp->x, py = ijp->y;
}

void Wheel::OnUp(InjectPoint*) {
	*status = 0;
}

void Wheel::OnMove(InjectPoint* ijp) {
	float32 a = (float32)(ijp->y - py);
	if (a < 0)a = 0 - a;
	a = (a / (Height() / 2.0f) * speed + 1.0f) * ijp->ay;
	mouse_event(MOUSEEVENTF_WHEEL, 0, 0, (int16)a, 0);
}

void Capskey::Code(byte c) {
	code = c;
}

void Button::Scale(FLOAT v) {
	Width(Width() * v);
	Height(Height() * v);
}

void Letter::Initialization() {
	parent->Handler()->Hotspot("key", this);
}

void Letter::OnPaint(IGraphics* g) {
	if (*status) {
		g->Brush(1.0f, 1.0f, 1.0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(.0f, .0f, .0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(1.0f, 1.0f, 1.0f, .85f);
		g->DrawTextW(*pt ? name : name1, Rx() - g->FontSize() / 2.0f, Ry() + g->FontSize() / 1.6f, Width(), Height());
		g->Brush(1.0f, 1.0f, 1.0f, .35f);
		g->DrawTextW(*pt ? name1 : name, Rx() + g->FontSize() / 2.0f, Ry() - g->FontSize() / 1.6f, Width(), Height());
	}
	else {
		g->Brush(.0f, .0f, .0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(1.0f, 1.0f, 1.0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(1.0f, 1.0f, 1.0f, .85f);
		g->DrawTextW(*pt ? name1 : name, Rx() - g->FontSize() / 2.0f, Ry() + g->FontSize() / 1.6f, Width(), Height());
		g->Brush(1.0f, 1.0f, 1.0f, .35f);
		g->DrawTextW(*pt ? name : name1, Rx() + g->FontSize() / 2.0f, Ry() - g->FontSize() / 1.6f, Width(), Height());
	}
}

PClass(SelectBox) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "select");
	}
	void* Create(pugi::xml_node & node) override {
		return new SelectBox;
	}
	void Child(void* o0, INodeParser * np, pugi::xml_node & node, void* o1) override {
		SelectBox* s = (SelectBox*)o0;
		s->Widget(np->ToWidget(o1));
	}
	void Init(pugi::xml_node & node, void* o, IDocumentParser * dp) override {
		SelectBox* t = (SelectBox*)o;
		if (node.attribute("name").empty()) return;
		IMergeNode* mn = dp->GetMerge(node.attribute("name").as_string());
		mn->Merge(t);
	}
	void Loaded(pugi::xml_node & node, void* o, IDocumentParser * dp) override {
		SelectBox* t = (SelectBox*)o;
		size_t id = node.attribute("default").as_uint(0);
		t->Default(id);
	}
};

PClass(FnKey) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "fn");
	}
	void* Create(pugi::xml_node & node) override {
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		return new FnKey((uint16)w, (uint16)h, node.text().as_string());
	}
	InitTemplate(FnKey);
};

PClass(Key) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "key");
	}
	void* Create(pugi::xml_node & node) override {
		const char* str = node.attribute("code").as_string();
		byte code = Keycode(str);
		if (code == 0) return nullptr;
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		Key* t = new Key((uint16)w, (uint16)h, node.text().as_string());
		t->Code(code);
		return t;
	}
	InitTemplate(Key);
};

PClass(MouseKey) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "mouse");
	}
	void* Create(pugi::xml_node & node) override {
		const char* str = node.attribute("code").as_string();
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		MouseKey* t = new MouseKey((uint16)w, (uint16)h, node.text().as_string());
		byte type = node.attribute("code").as_uint(1);
		if (type == 1)
			t->Code(MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP);
		else
			t->Code(MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP);
		return t;
	}
	InitTemplate(MouseKey);
};

PClass(Wheel) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "wheel");
	}
	void* Create(pugi::xml_node & node) override {
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		Wheel* t = new Wheel((uint16)w, (uint16)h, node.text().as_string());
		t->Speed(node.attribute("speed").as_float(2.0));
		return t;
	}
	InitTemplate(Wheel);
};

PClass(Letter) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "letter");
	}
	void* Create(pugi::xml_node & node) override {
		const char* str = node.attribute("code").as_string();
		byte code = Keycode(str);
		if (code == 0) return nullptr;
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		Letter* t = new Letter(
			(uint16)w, (uint16)h,
			node.text().as_string(),
			node.attribute("text").as_string()
		);
		t->Code(code);
		return t;
	}
	InitTemplate(Letter);
};

PClass(Capskey) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "caps");
	}
	void* Create(pugi::xml_node & node) override {
		const char* str = node.attribute("code").as_string();
		byte code = Keycode(str);
		if (code == 0) return nullptr;
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		Capskey* t = new Capskey((uint16)w, (uint16)h, node.text().as_string());
		t->Code(code);
		return t;
	}
	InitTemplate(Capskey);
};

PClass(Keypad, 1) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "keypad");
	}
	void* Create(pugi::xml_node & node) override {
		Keypad* k = new Keypad;
		k->Size(0.1, 0.1);
		return k;
	}
	void Init(pugi::xml_node & node, void* o, IDocumentParser * dp) override {
		Keypad* k = (Keypad*)o;
		k->Scale(node.attribute("scale").as_float(1));
		k->X(ScreenX(k->Width(), &node.attribute("x")));
		k->Y(ScreenY(k->Height(), &node.attribute("y")));
	}
	void Child(void* o0, INodeParser * np, pugi::xml_node & node, void* o1) override {
		Keypad* k = (Keypad*)o0;
		Hotspot* h = np->ToHotspot(o1);
		float32 x = node.attribute("x").as_float(46.0f);
		float32 y = node.attribute("y").as_float(46.0f);
		k->Hotspot((int16)x, (int16)y, h);
	}
};

PClass(Button, 1) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "button");
	}
	void* Create(pugi::xml_node & node) override {
		Button* k = new Button(node.text().as_string());
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		k->Size(w, h);
		k->Scale(node.attribute("scale").as_float(1));
		k->X(ScreenX(k->Width(), &node.attribute("x")));
		k->Y(ScreenY(k->Height(), &node.attribute("y")));
		return k;
	}
	void Init(pugi::xml_node & node, void* o, IDocumentParser * dp) override {
		Button* t = (Button*)o;
		if (node.attribute("name").empty()) return;
		IMergeNode* mn = dp->GetMerge(node.attribute("name").as_string());
		t->Ptr(mn->Ptr());
		mn->Merge(t);
	}
};

RClass(SelectBox);
RClass(Button);
PChild(SelectBox, Button);
RClass(Keypad);
PChild(SelectBox, Keypad);
RClass(Key);
PChild(Keypad, Key);
RClass(FnKey);
PChild(Keypad, FnKey);
RClass(MouseKey);
PChild(Keypad, MouseKey);
RClass(Wheel);
PChild(Keypad, Wheel);
RClass(Letter);
PChild(Keypad, Letter);
RClass(Capskey);
PChild(Keypad, Capskey);

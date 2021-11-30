#include "module.h"
#include "helper.h"

#pragma warning(disable: 4003)

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

BinKey::~BinKey() {
	SafeDeletes(downbin);
	SafeDeletes(upbin);
}

void BinKey::Runbin(byte* bin) {
	size_t i = 0;
	while (byte b = bin[i]) {
		switch (b) {
		case 1:
			keybd_event(bin[i + 1], 0, bin[i + 2], 0);
			i += 3;
			break;
		case 2:
			mouse_event(bin[i + 1], 0, 0, 0, EVENT_FID);
			i += 2;
			break;
		default:
			return;
		}
	}
}

void BinKey::DownCode(byte* d) {
	downbin = d;
}

void BinKey::UpCode(byte* d) {
	upbin = d;
}

FnKey::~FnKey() {
	BinKey::~BinKey();
}


void FnKey::OnDown(InjectPoint*) {
	*status = 1;
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
	if (downbin) Runbin(downbin);
}

void FnKey::OnUp(InjectPoint*) {
	*status = 0;
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
	if (upbin) Runbin(upbin);
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
	mouse_event(dcode, 0, 0, 0, EVENT_FID);
	if (HasMerge()) CallMerge(status); else parent->OnPaint();
}

void MouseKey::OnUp(InjectPoint*) {
	*status = 0;
	mouse_event(ucode, 0, 0, 0, EVENT_FID);
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

Button::~Button() {
	BinKey::~BinKey();
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
		g->DrawTextW(*pt ? name : name1, Rx(), Ry(), Width(), Height());
	}
	else {
		g->Brush(.0f, .0f, .0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(1.0f, 1.0f, 1.0f, .5f);
		g->FillRectangle(Rx(), Ry(), Width(), Height());
		g->Brush(.0f, .0f, .0f, .85f);
		g->DrawTextW(*pt ? name1 : name, Rx(), Ry(), Width(), Height());
	}
}

void Symbol::OnPaint(IGraphics* g) {
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

namespace BinParseer {
	void Keybin(std::vector<byte>& b, byte c, byte t = 3) {
		if ((t & 2) != 0) {
			b.push_back(1);
			b.push_back(c);
			b.push_back(0);
		}
		if ((t & 1) != 0) {
			b.push_back(1);
			b.push_back(c);
			b.push_back(KEYEVENTF_KEYUP);
		}
	}
	void Mousebin(std::vector<byte>& b, byte c, byte t = 3) {
		if ((t & 2) != 0) {
			b.push_back(2);
			b.push_back(c);
		}
		if ((t & 1) != 0) {
			b.push_back(2);
			b.push_back(c * 2);
		}
	}
	byte Parsevalue(std::string& str, std::vector<byte>& b) {
		size_t i = str.find(':', 0);
		std::string name = str;
		byte t = 0;
		if (i >= str.size()) {
			t = 3;
		}
		else {
			name = str.substr(0, i);
			std::string val = str.substr(i + 1, str.size());
			if (val == "0") t = 1;
			else if (val == "1")t = 2;
			else if (val == "2")t = 3;
		}
		if (str == "m1") Mousebin(b, MOUSEEVENTF_LEFTDOWN, t);
		else if (str == "m2") Mousebin(b, MOUSEEVENTF_RIGHTDOWN, t);
		else if (str == "m3") Mousebin(b, MOUSEEVENTF_MIDDLEDOWN, t);
		else {
			int16 n = Keycode(name);
			if (n == -1||n>0xFF) return 0;
			Keybin(b, (byte)n, t);
		}
		return 1;
	}
	byte* Parsecode(const char* c) {
		size_t len = strlen(c);
		std::string str = "";
		std::vector<byte> b;
		for (size_t i = 0; i < len; i++) {
			if (c[i] == ';') {
				if (!Parsevalue(str, b))return nullptr;
				str = "";
				continue;
			}
			str += c[i];
		}
		if (!Parsevalue(str, b) || b.size() == 0)return nullptr;
		byte* r = new byte[b.size() + 1];
		for (size_t i = 0; i < b.size(); i++) r[i] = b[i];
		r[b.size()] = 0;
		b.clear();
		return r;
	}
}

PClass(WidgetBox) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "space");
	}
	void* Create(pugi::xml_node & node) override {
		return new WidgetBox;
	}
	void Child(void* o0, INodeParser * np, pugi::xml_node & node, void* o1) override {
		WidgetBox* s = (WidgetBox*)o0;
		s->Widget(np->ToWidget(o1));
	}
};

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
	void* Create(pugi::xml_node& node) override {
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		byte* d1 = nullptr, * d2 = nullptr;
		if (!node.attribute("down").empty()) {
			d1 = BinParseer::Parsecode(node.attribute("down").as_string());
			if (!d1) return nullptr;
		}
		if (!node.attribute("up").empty()) {
			d1 = BinParseer::Parsecode(node.attribute("up").as_string());
			if (!d1) return nullptr;
		}
		FnKey* fn = new FnKey((uint16)w, (uint16)h, node.text().as_string());
		fn->DownCode(d1);
		fn->UpCode(d2);
		return fn;
	}
	InitTemplate(FnKey);
};

PClass(Key) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "key");
	}
	void* Create(pugi::xml_node& node) override {
		int16 code = CodeOrKey(&node);
		if (code < 0 || code>0xFF) return nullptr;
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		Key* t = new Key((uint16)w, (uint16)h, node.text().as_string());
		t->Code((byte)code);
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
	void* Create(pugi::xml_node& node) override {
		int16 code = CodeOrKey(&node);
		if (code < 0 || code>0xFF) return nullptr;
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		Letter* t = new Letter(
			(uint16)w, (uint16)h,
			node.text().as_string(),
			node.attribute("text").as_string()
		);
		t->Code((byte)code);
		return t;
	}
	InitTemplate(Letter);
};

PClass(Symbol) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "symbol");
	}
	void* Create(pugi::xml_node& node) override {
		int16 code = CodeOrKey(&node);
		if (code < 0 || code >0xFF) return nullptr;
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		Symbol* t = new Symbol(
			(uint16)w, (uint16)h,
			node.text().as_string(),
			node.attribute("text").as_string()
		);
		t->Code((byte)code);
		return t;
	}
	InitTemplate(Symbol);
};

PClass(Capskey) {
public:
	byte Is(pugi::xml_node & node)override {
		return Equal(node.name(), "caps");
	}
	void* Create(pugi::xml_node& node) override {
		int16 code = CodeOrKey(&node);
		if (code < 0 || code >0xFF) return nullptr;
		float32 w = node.attribute("width").as_float(46.0f);
		float32 h = node.attribute("height").as_float(46.0f);
		Capskey* t = new Capskey((uint16)w, (uint16)h, node.text().as_string());
		t->Code((byte)code);
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
		k->Size(0.1f, 0.1f);
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
RClass(WidgetBox);
RClass(SelectBox);
PChild(SelectBox, SelectBox);
PChild(SelectBox, WidgetBox);
PChild(WidgetBox, SelectBox);
RClass(Button);
PChild(SelectBox, Button);
PChild(WidgetBox, Button);
RClass(Keypad);
PChild(SelectBox, Keypad);
PChild(WidgetBox, Keypad);
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
RClass(Symbol);
PChild(Keypad, Symbol);
RClass(Capskey);
PChild(Keypad, Capskey);


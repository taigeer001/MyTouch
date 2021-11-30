#include "parser.h"
#include "pugixml.hpp"

std::map<std::string, byte> keycode = {
	{"backspace",0x08}, {"tab",0x09}, {"clear",0x0C}, {"enter",0x0D}, {"shift",0x10},
	{"ctrl",0x11}, {"alt",0x12}, {"pause",0x13}, {"caps",0x14}, {"esc",0x1B},
	{"spacebar",0x20}, {"page_up",0x21}, {"page_down",0x22}, {"end",0x23}, {"home",0x24},
	{"left",0x25}, {"up",0x26}, {"right",0x27}, {"down",0x28},
	{"select",0x29}, {"print",0x2A}, {"execute",0x2B}, {"print_screen",0x2C}, {"ins",0x2D},
	{"del",0x2E}, {"help",0x2F}, {"0",0x30}, {"1",0x31}, {"2",0x32}, {"3",0x33}, {"4",0x34},
	{"5",0x35}, {"6",0x36}, {"7",0x37}, {"8",0x38}, {"9",0x39}, {"a",0x41}, {"b",0x42},
	{"c",0x43}, {"d",0x44}, {"e",0x45}, {"f",0x46}, {"g",0x47}, {"h",0x48}, {"i",0x49},
	{"j",0x4A}, {"k",0x4B}, {"l",0x4C}, {"m",0x4D}, {"n",0x4E}, {"o",0x4F}, {"p",0x50},
	{"q",0x51}, {"r",0x52}, {"s",0x53}, {"t",0x54}, {"u",0x55}, {"v",0x56}, {"w",0x57},
	{"x",0x58}, {"y",0x59}, {"z",0x5A}, {"numpad_0",0x60}, {"numpad_1",0x61}, {"numpad_2",0x62},
	{"numpad_3",0x63}, {"numpad_4",0x64}, {"numpad_5",0x65}, {"numpad_6",0x66}, {"numpad_7",0x67},
	{"numpad_8",0x68}, {"numpad_9",0x69}, {"multiply_key",0x6A}, {"add_key",0x6B}, {"separator_key",0x6C},
	{"subtract_key",0x6D}, {"decimal_key",0x6E}, {"divide_key",0x6F}, {"F1",0x70}, {"F2",0x71},
	{"F3",0x72}, {"F4",0x73}, {"F5",0x74}, {"F6",0x75}, {"F7",0x76}, {"F8",0x77}, {"F9",0x78},
	{"F10",0x79}, {"F11",0x7A}, {"F12",0x7B}, {"F13",0x7C}, {"F14",0x7D}, {"F15",0x7E}, {"F16",0x7F},
	{"F17",0x80}, {"F18",0x81}, {"F19",0x82}, {"F20",0x83}, {"F21",0x84}, {"F22",0x85}, {"F23",0x86},
	{"F24",0x87}, {"num_lock",0x90}, {"scroll_lock",0x91}, {"left_shift",0xA0}, {"right_shift",0xA1},
	{"left_control",0xA2}, {"right_control",0xA3}, {"left_menu",0xA4}, {"right_menu",0xA5},
	{"browser_back",0xA6}, {"browser_forward",0xA7}, {"browser_refresh",0xA8}, {"browser_stop",0xA9},
	{"browser_search",0xAA}, {"browser_favorites",0xAB}, {"browser_start_and_home",0xAC},
	{"volume_mute",0xAD}, {"volume_Down",0xAE}, {"volume_up",0xAF}, {"next_track",0xB0},
	{"previous_track",0xB1}, {"stop_media",0xB2}, {"play/pause_media",0xB3}, {"start_mail",0xB4},
	{"select_media",0xB5}, {"start_application_1",0xB6}, {"start_application_2",0xB7},
	{"attn_key",0xF6}, {"crsel_key",0xF7}, {"exsel_key",0xF8}, {"play_key",0xFA},
	{"zoom_key",0xFB}, {"clear_key",0xFE}, {"+",0xBB}, {",",0xBC}, {"-",0xBD}, {".",0xBE},
	{"/",0xBF}, {"`",0xC0}, {";",0xBA}, {"[",0xDB}, {"\\",0xDC}, {"]",0xDD}, {"'",0xDE}
};

int16 Keycode(const char* n) {
	std::string t(n);
	if (keycode.count(t) == 0) return -1;
	return keycode[t];
}

int16 Keycode(std::string& t) {
	if (keycode.count(t) == 0) return -1;
	return keycode[t];
}

int16 CodeOrKey(pugi::xml_node* node) {
	if (node->attribute("key").empty()) {
		return node->attribute("code").as_uint(0);
	}
	const char* str = node->attribute("key").as_string();
	int16 code = Keycode(str);
#ifndef _WINDOWS
	std::cout << str << "," << code << "" << std::endl;
#endif // !_WINDOWS
	return code;
}

float32 ScreenY(float32 height, pugi::xml_attribute* attr) {
	const char* str = attr->as_string();
	float32 sy = (float32)GetSystemMetrics(SM_CYSCREEN);
	if (Equal(str, "center")) {
		return (sy - height) / 2.0f;
	}
	else if (Equal(str, "top")) {
		return 0;
	}
	else if (Equal(str, "bottom")) {
		return sy - height;
	}
	else if (str[strlen(str) - 1] == '%') {
		return attr->as_float() / 100.0f * sy;
	}
	else {
		return attr->as_float();
	}
}

float32 ScreenX(float32 width, pugi::xml_attribute* attr) {
	const char* str = attr->as_string();
	float32 sx = (float32)GetSystemMetrics(SM_CXSCREEN);
	if (Equal(str, "center")) {
		return (sx - width) / 2.0f;
	}
	else if (Equal(str, "left")) {
		return 0;
	}
	else if (Equal(str, "right")) {
		return sx - width;
	}
	else if (str[strlen(str) - 1] == '%') {
		return attr->as_float() / 100.0f * sx;
	}
	else {
		return attr->as_float();
	}
}

INodeParser** IDocumentParser::list = nullptr;
size_t IDocumentParser::listlen = 0;

INodeParser* IDocumentParser::Parser(pugi::xml_node& node) {
	for (size_t i = 0; i < listlen; i++) {
		if (list[i]->Is(node))return list[i];
	}
	return nullptr;
}

void IDocumentParser::Node(pugi::xml_node& tn, IDocumentParser::D* d) {
	for (pugi::xml_node node : tn.children()) {
		INodeParser* np = Parser(node);
		if (!np) continue;
		void* o = np->Create(node);
		if (!o) continue;
		object.push_back(new D(node, np, o));
#ifndef _WINDOWS
		if (d) {
			//std::cout << node.name() << "," << d->n.name();
			//std::cout << " ischild: " << (int)d->p->IsChild(np, node, o) << std::endl;
		}
#endif // !_WINDOWS
		if (d && d->p->IsChild(np, node, o)) d->p->Child(d->o, np, node, o);
		if (!np->HasChild()) continue;
		Node(node, object.back());
	}
}

void IDocumentParser::Analysis() {
	Node(doc);
	for (size_t i = 0; i < object.size(); i++) {
		object[i]->p->Init(object[i]->n, object[i]->o, this);
	}
	for (size_t i = 0; i < object.size(); i++) {
		if (!object[i]->p->IsWidget())continue;
		widget.push_back(object[i]->p->ToWidget(object[i]->o));
	}
}

void IDocumentParser::Loaded() {
	for (size_t i = 0; i < object.size(); i++) {
		object[i]->p->Loaded(object[i]->n, object[i]->o, this);
	}
}

byte IDocumentParser::Open(const char* file) {
	return !!doc.load_file(file);
}
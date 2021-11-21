#pragma once

#include <functional>
#include <map>
#include <vector>
#include "widget.h"
#include "type.h"
#include "pugixml.hpp"

int16 Keycode(const char*);

int16 CodeOrKey(pugi::xml_node*);

float32 ScreenX(float32, pugi::xml_attribute*);

float32 ScreenY(float32, pugi::xml_attribute*);

class IMergeNode {
private:
	size_t length = 0;
	IMergeNode** list = nullptr;
	IMergeNode* _this = this;
	byte has = 0;
public:
	virtual int32* Ptr() { return nullptr; }
	virtual void Ptr(int32*) {}
	virtual void OnMerge(uint16, void*) {}
	forceinline byte HasMerge() {
		return _this->has;
	}
	void Merge(IMergeNode* mn, byte add = 1) {
		if (add) {
			for (size_t i = 0; i < length; i++) {
				if (list[i] == mn)return;
			}
			list = Extension<IMergeNode>(list, length);
			list[length++] = mn;
		}
		mn->_this = _this, has = 1;
	}
	void CallMerge(void* o = nullptr, uint16 id = 0) {
		if (_this != this) {
			_this->CallMerge(o, id);
			return;
		}
		for (size_t i = 0; i < length; i++) {
			list[i]->OnMerge(id, o);
		}
	}
};
class MergeNode : public IMergeNode {
protected:
	int32 value = 0;
	int32* ptr = &value;
public:
	int32* Ptr() override { return ptr; }
	void Ptr(int32* p) override { ptr = p; }
};

#define PClass(name,type)  \
class NodeParser_##name##_M:public INodeParser{\
public:\
	byte IsWidget() override {\
		return 0##type##;\
	}\
	Hotspot* ToHotspot(void* o) override {\
		return (Hotspot*)(##name##*)o;\
	}\
	::IWidget* ToWidget(void* o) override {\
		return (Widget*)(##name##*)o;\
	}\
	IMergeNode* ToMerge(void* o) override {\
		return (IMergeNode*)(##name##*)o;\
	}\
};\
class NodeParser_##name##:public NodeParser_##name##_M

#define RClass(name) static byte NodeParser_##name##_VAR = \
IDocumentParser::RegNode<NodeParser_##name##>()

#define PChild(p, c) static byte NodeParser_##p##_##c##_CHILD = \
IDocumentParser::Child<NodeParser_##p##,NodeParser_##c##>()

class IDocumentParser;

class INodeParser {
private:
	byte haschild = 0;
	uint16 id = 0;
	std::vector<uint16> child;
	std::string name;
public:
	uint16 Id() { return id; }
	std::string Name() { return name; }
	virtual byte IsWidget() = 0;
	virtual byte Is(pugi::xml_node&) = 0;
	virtual void* Create(pugi::xml_node&) = 0;
	virtual Hotspot* ToHotspot(void* o) = 0;
	virtual ::IWidget* ToWidget(void* o) = 0;
	virtual IMergeNode* ToMerge(void* o) = 0;
	virtual void Child(void*, INodeParser*, pugi::xml_node&, void*) {}
	virtual void Init(pugi::xml_node&, void*, IDocumentParser* dp) {}
	virtual void Loaded(pugi::xml_node&, void*, IDocumentParser* dp) {}
	virtual byte HasChild() { return haschild; }
	virtual byte IsChild(INodeParser* np, pugi::xml_node&, void*) {
		for (size_t i = 0; i < child.size(); i++) {
			if (np->id == child[i]) return 1;
		}
		return 0;
	}
	static uint16 AllotId(INodeParser* np, std::string n) {
		static uint16 gid = 0;
		np->id = gid++;
		np->name = n;
		return gid;
	}
	void AddChild(uint16 i) {
		child.push_back(i);
		haschild = 1;
	}
};

class IDocumentParser {
protected:
	static size_t listlen;
	static INodeParser** list;
	struct D {
		pugi::xml_node n;
		INodeParser* p = nullptr;
		void* o = nullptr;
		D(pugi::xml_node a, INodeParser* b, void* c) :n(a), p(b), o(c) {}
	};
	pugi::xml_document doc;
	float32 speed = 2.0f;
	std::map<std::string, IMergeNode*> merge;
	std::vector<D*> object;
	std::vector<IWidget*> widget;
	INodeParser* Parser(pugi::xml_node&);
	virtual void Node(pugi::xml_node&, D* = nullptr);
public:
	virtual ::IWidget* operator[](size_t n) { return widget[n]; };
	virtual size_t Widget() { return widget.size(); };
	virtual void Analysis();
	virtual void Loaded();
	virtual byte Open(const char*);
	virtual float32 Speed() { return speed; }
	virtual IMergeNode* GetMerge(std::string n) {
		if (merge.count(n) == 0) {
			merge[n] = new MergeNode;
		}
		return merge[n];
	}
	static INodeParser* FindNode(std::string n) {
		for (size_t i = 0; i < listlen; i++) {
			if (list[i]->Name() == n) return list[i];
		}
		return nullptr;
	}
	template<class T> static std::string GetClassName() {
		std::string s = typeid(T).name();
		size_t d0 = s.find(' ') + 1;
		size_t d1 = s.find(' ', d0);
		if (d0 == d1) return s;
		return s.substr(d0, d1 - d0);
	}
	template<class T> static byte RegNode() {
		INodeParser* np = new T;
		INodeParser::AllotId(np, GetClassName<T>());
		list = Extension(list, listlen);
		list[listlen++] = np;
		return 0;
	}
	template<class A, class T> static byte Child() {
		std::string n1 = GetClassName<A>();
		std::string n2 = GetClassName<T>();
#ifndef _WINDOWS
		std::cout << "child: " << n1 << "," << n2 << std::endl;
#endif // !_WINDOWS
		INodeParser* p = FindNode(n1);
		if (!p) return 0;
		INodeParser* c = FindNode(n2);
		if (!c) return 0;
#ifndef _WINDOWS
		std::cout << "child id: " << p->Id() << "," << c->Id() << std::endl;
#endif // !_WINDOWS
		p->AddChild(c->Id());
		return 0;
	}
};

class DocMakeFactory :public Factory {
protected:
	IDocumentParser* doc = nullptr;
	byte loaded = 0;
public:
	void Document(IDocumentParser* dp) {
		doc = dp;
	}
	void Start() override {
		if (!loaded) {
			loaded = 1;
			doc->Analysis();
			for (size_t i = 0; i < doc->Widget(); i++) {
				Widget((*doc)[i]);
			}
			doc->Loaded();
		}
		Factory::Start();
	}
};

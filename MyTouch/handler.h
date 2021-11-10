#pragma once

#include "type.h"
#include <iostream>

class InjectBlock :public IInject {
protected:
	size_t length = 0;
	IHotspot** hotspot = nullptr;
public:
	virtual void Hotspot(IHotspot*) override;
	virtual const char* TypeName() override { return "block"; };
	virtual InjectData* StartInput(PointTable*, PointInfo*, TouchPoint*) override;
};

class InjectKey :public InjectBlock {
protected:
	InjectData cache;
	struct {
		INT16 x = 0, y = 0;
		UINT64 time = 0;
		IHotspot* hotspot = nullptr;
	} table[10];
public:
	virtual const char* TypeName() override { return "key"; };
	virtual InjectData* StartInput(PointTable*, PointInfo*, TouchPoint*) override;
	virtual InjectData* Input(PointTable*, PointInfo*, TouchPoint*) override;
	virtual InjectData* CloseInput(PointTable*, PointInfo*, TouchPoint*) override;
	virtual void Hotspot(IHotspot*) override;
};

class InjectJoystick :public InjectKey {
public:
	virtual const char* TypeName() override { return "joystick"; };
	virtual InjectData* StartInput(PointTable*, PointInfo*, TouchPoint*) override;
	virtual InjectData* Input(PointTable*, PointInfo*, TouchPoint*) override;
	virtual InjectData* CloseInput(PointTable*, PointInfo*, TouchPoint*) override;
};

class InjectMouse :public IInject, StackId {
protected:
	struct Status {
		uint16 id = 0;
		byte code = 0;
		int16 x = 0, y = 0;
	};
	InjectData cache;
	Status* sta = nullptr, * zero = nullptr;
	forceinline BYTE PointType(TouchPoint* tp) {
		return (zero->y - (zero->x - tp->x)) > tp->y;
	}
public:
	InjectMouse();
	~InjectMouse();
	virtual const char* TypeName() override { return "mouse"; };
	virtual InjectData* StartInput(PointTable*, PointInfo*, TouchPoint*) override;
	virtual InjectData* Input(PointTable*, PointInfo*, TouchPoint*)  override;
	virtual InjectData* CloseInput(PointTable*, PointInfo*, TouchPoint*)  override;
};

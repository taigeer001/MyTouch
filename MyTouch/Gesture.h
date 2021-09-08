#pragma once
#include "TouchData.h"

#define TOUCHDOWN 231
#define TOUCHUP 228

enum ActionType
{
	None,
	TouchId0,
	TouchDown,
	TouchUp,
	To,
	Move,
	LeftDown,
	RightDown,
	LeftUp,
	RightUp,
};

struct Action {
	ActionType Type;
	void* Data;
};

struct TouchNode
{
	BYTE Sta;
	BYTE Hold;
	BYTE TimeType;
	BYTE Oth;
	BYTE Zero;
	BYTE Zero1;
	BYTE Zero2;
	BYTE Zero3;
};

struct  TouchPoint
{
	int Idx;
	UINT64 Hash;
	UINT16 Id;
	UINT16 X;
	UINT16 Y;
	UINT32 Time;
};

class Gesture
{
private:
	TouchNode node[10];
	TouchPoint point[10];
	TouchPoint* office[10];
	TouchPoint* start;
	TouchPoint* end;
	UINT16 length = 0;

	int GetPoint(UINT16 id);
	TouchNode NewTouchNode(int, TouchData*, Touch*);
	TouchPoint NewTouchPoint(int, TouchData*, Touch*);
	TouchPoint* PushPoint(TouchData*, Touch*);
	void ClearIdx(TouchData* , Touch* );
	BYTE CalcTimeType(UINT32);
	BYTE CalcHold(Touch*, TouchPoint*);
	UINT64 CalcHash(TouchNode*);

	ActionType First(TouchPoint*, Touch*);
	ActionType Second(TouchPoint*, Touch*);
	ActionType Other(TouchPoint*, Touch*);
public:
	Gesture();
	Action Action(TouchData *td);
};


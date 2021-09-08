#pragma once
#include <Windows.h>

/*struct TouchPoint
{
    BYTE status;
    BYTE a;
    UINT16 id;
    UINT16 x1;
    UINT16 x2;
    UINT16 y1;
    UINT16 y2;
    UINT32 area;
    UINT32 zero;
};

struct TouchPoints
{
    BYTE zero;
    UINT16 id;
    TouchPoint point[10];
    UINT16 length;
    UINT16 a;
    UINT32 time;
};*/

class Touch {
public:
    BYTE Sta;
    BYTE a;
    UINT16 Id;
    UINT16 X;
private:
    UINT16 x;
public:
    UINT16 Y;
private:
    UINT16 y;
public:
    UINT32 Area;
private:
    UINT32 zero;
};

class TouchData
{
private:
    BYTE zero;
public:
    UINT16 Id;
private:
    Touch point[10];
public:
    UINT16 Length;
private:
    UINT16 a;
public:
    UINT32 Time;
    static TouchData& From(RAWINPUT*);
    Touch operator[](int i) {
        return point[i];
    }
};

typedef TouchData* PTouchData;
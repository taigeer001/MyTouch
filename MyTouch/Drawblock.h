#pragma once

#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;
#include <vector>

class Drawblocks;

class Drawblock
{
private:
    Drawblocks* parent;
    POINT loc;
    SIZE size;
    HWND hwnd;
    HDC hdc, memdc;
public:
    Drawblock(Drawblocks*, HWND);
    Drawblock(Drawblocks*, HWND, POINT, SIZE);
    BOOL Refresh(int);
    ~Drawblock();
};

class Drawblocks
{
private:
    std::vector<void*> res;
public:
    Drawblocks();
    template <typename T>
    T GetResource(int);
    int AppendRes(void*);
    int LoadImage(const WCHAR*);
    Drawblock* NewBlock();
    Drawblock* NewBlock(int, int);
    Drawblock* NewBlock(int, int, int, int);
    Drawblock* NewBlock(int, int, SIZE);
    Drawblock* NewBlock(POINT, int, int);
    Drawblock* NewBlock(POINT, SIZE);
    ~Drawblocks();
};


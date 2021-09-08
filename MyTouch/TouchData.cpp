#include "TouchData.h"
#include <iostream>
using namespace std;

TouchData& TouchData::From(RAWINPUT* i)
{
	return *(TouchData*)i->data.hid.bRawData;
}

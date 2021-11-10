#include "plant.h"
#include <iostream>
using namespace std;

int main()
{
	try {
		DocMakeFactory fa;
		DebugInput dbi;
		//DebugDev dbi;
		//fa.Input(&dbi);
		InjectMouse ijm;
		InjectKey ijk;
		DocumentParser dp;
		dp.Open("C:\\Users\\zr\\Downloads\\1\\config.xml");
		fa.Default();
		fa.Inject(&ijk);
		fa.Inject(&ijm);
		fa.Document(&dp);
		fa.Start();
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	catch (const WCHAR* e)
	{
		wcout << e << endl;
	}

}
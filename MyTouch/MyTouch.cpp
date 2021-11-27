#include "plant.h"
#include <iostream>
using namespace std;

#ifndef _WINDOWS
int main()
{
	try {
		DocMakeFactory fa;
		DebugInput dbi;
		//DebugDev dbi;
		//fa.Input(&dbi);
		InjectMouse ijm;
		//TestMouse ijm;
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
#else
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	if (FindWindow(GetTouchClass(), 0)) {
		MessageBox(0, L"Only one can be run", L"error", MB_ICONERROR | MB_TOPMOST);
		exit(0);
	}
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	try {
		DocMakeFactory fa;
		InjectMouse ijm;
		InjectKey ijk;
		DocumentParser dp;
		if (!dp.Open("C:\\Users\\zr\\Downloads\\1\\config.xml"))
			throw L"open xml error";
		fa.Default();
		fa.Inject(&ijk);
		fa.Inject(&ijm);
		fa.Document(&dp);
		fa.Start();
	}
	catch (const std::exception& e) {
		MessageBoxA(0, e.what(), "Error", MB_TOPMOST | MB_ICONERROR | MB_SYSTEMMODAL);
	}
	catch (const WCHAR* e) {
		MessageBox(0, e, TEXT("Error"), MB_TOPMOST | MB_ICONERROR | MB_SYSTEMMODAL);
	}
	return 0;
}
#endif
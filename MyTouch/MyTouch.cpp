
#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include "TouchData.h"
#include "Gesture.h"
#include "Drawblock.h"
using namespace std;
 

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

const static LPCWSTR szClass = L"Drawblock";
const static LPCWSTR szTitle = L"Demo-1";

int main()
{
    Drawblocks* dbs = new Drawblocks();
    
    HINSTANCE hIns = ::GetModuleHandle(0);
    /*WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hIns;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hIconSm = 0;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.lpfnWndProc = WndProc;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szClass;

    if (!RegisterClassEx(&wc)) exit(0);*/
    
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD styleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

    HWND hWnd = CreateWindowEx(styleEx,szClass, szTitle, style,
        0, 0, 1, 1,
        0, 0, hIns, 0);

    if (!hWnd)
    {
        
        cout << "false"<< GetLastError() << endl;
        return FALSE;
    }

    UpdateWindow(hWnd);
    ShowWindow(hWnd, SW_SHOW);

    std::cout << "Hello World!\n";
    UINT numDevices = 0;
    GetRawInputDeviceList(NULL, &numDevices, sizeof(RAWINPUTDEVICELIST));
    PRAWINPUTDEVICELIST devices = new RAWINPUTDEVICELIST[numDevices];
    GetRawInputDeviceList(devices, &numDevices, sizeof(RAWINPUTDEVICELIST));

    for (size_t i = 0; i < numDevices; i++)
    {
        RID_DEVICE_INFO rdi;
        rdi.cbSize = sizeof(rdi);
        UINT cbSize = sizeof(rdi);
        GetRawInputDeviceInfo(devices[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize);
        if (rdi.hid.usUsage == 4) {
            RAWINPUTDEVICE device;
            device.usUsagePage = rdi.hid.usUsagePage;
            device.usUsage = rdi.hid.usUsage;
            cout << rdi.hid.usUsage << " " << rdi.hid.usUsagePage << endl;
            device.dwFlags = RIDEV_INPUTSINK;
            device.hwndTarget = hWnd;
            if (RegisterRawInputDevices(&device, 1, sizeof(device)) == FALSE) {
                wcout << L"error "<<GetLastError() << endl;
            }
        }
    }
    Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken;
    GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

    MSG msg;
    int n = 0;
    Gesture* g = new Gesture();
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        switch (msg.message) {
        case WM_INPUT:
            UINT iSize = 0;
            UINT result = GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &iSize, sizeof(RAWINPUTHEADER));
            LPBYTE lpInput = new BYTE[iSize];
            result = GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpInput, &iSize, sizeof(RAWINPUTHEADER));
            RAWINPUT* pInput = (RAWINPUT*)lpInput;
            TouchData td = TouchData::From(pInput);
            //TouchPoints *tp = (TouchPoints*)pInput->data.hid.bRawData;
            //cout << "s:" << (UINT)tp->point[0].status << endl;
            ActionType at = g->Action(&td).Type;
            //cout << "s:" << (UINT)td[0].Sta << endl;
            //cout << n++;//<<"L:"<<td.Length;
            switch (at)
            {
            case None:
                //cout << ":None" << endl;
                break;
            case To:
                cout << ":To" << endl;
                break;
            case Move:
                cout << ":Move" << endl;
                break;
            case LeftDown:
                cout << ":LeftDown" << endl;
                break;
            case RightDown:
                cout << ":RightDown" << endl;
                break;
            case LeftUp:
                cout << ":LeftUp" << endl;
                break;
            case RightUp:
                cout << ":RightUp" << endl;
                break;
            default:
                break;
            }
            //Gdiplus::Pen pen(Gdiplus::Color(255,0,0));
            //hWnd = GetDesktopWindow();
            HDC hdc = ::GetDC(0);
            //RoundRect(hdc, 100, 100, 500, 500, 300,300);
            //cout << hdc << endl;
            Gdiplus::Graphics *gh= Gdiplus::Graphics::FromHDC(hdc);
            Gdiplus::Image image(L"C:\\Users\\zr\\Downloads\\1.png", FALSE);
            
            gh->DrawImage(&image, 100, 100);
            //gh->DrawRectangle(&pen, 300,300,100,100 );
            gh->ReleaseHDC(hdc);
            
            //ValidateRect(hWnd, NULL);
            delete(lpInput);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_PAINT:
        break;
    case WM_QUIT:
    case WM_DESTROY:
        cout << "exit" << endl;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

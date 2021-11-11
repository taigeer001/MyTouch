#include <mutex>
#include <iostream>
#include <fstream>
#include "win.h"

LPCWSTR szTitle = L"MyTouch", szTouchClass = L"ZrTouchClass", szDrawingClass = L"ZrDrawingClass";

LRESULT CALLBACK QuitProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

IInput* input = nullptr;
LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        if (!input) break;
        input->Callback()->OnTimer();
        break;
    case WM_DISPLAYCHANGE:
        if (!input) break;
        input->Callback()->OnDisplayChange();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

WNDCLASSEXW* DefaultClass() {
    WNDCLASSEXW* wcex = new WNDCLASSEXW;
    wcex->cbSize = sizeof(WNDCLASSEX);
    wcex->style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_NOCLOSE;
    wcex->cbClsExtra = 0;
    wcex->cbWndExtra = 0;
    wcex->hInstance = GetModuleHandle(0);
    wcex->hIcon = 0;
    wcex->hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex->hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex->lpszMenuName = L"";
    wcex->hIconSm = 0;
    return wcex;
}

HWND DrawingWindow() {
    HINSTANCE hInstance = GetModuleHandle(0);
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        szDrawingClass, szTitle, WS_VISIBLE | WS_POPUP,
        0, 0, 1, 1, GetDesktopWindow(), nullptr, hInstance, nullptr
    );
    if (!hwnd) {
        return 0;
    }
    return hwnd;
}

std::mutex Mutex;
HWND MsgWindow() {
    Mutex.lock();
    static BOOL msginstance = FALSE;
    if (msginstance) ErrorAndExit(L"[MsgWindow] can only be called once.");
    HINSTANCE hInstance = GetModuleHandle(0);
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        szTouchClass, szTitle, WS_VISIBLE | WS_POPUP,
        0, 0, 1, 1, GetDesktopWindow(), nullptr, hInstance, nullptr
    );
    if (hwnd) msginstance = TRUE;
    Mutex.unlock();
    return hwnd;
}

int WindowsInit() {
    WNDCLASSEXW* wcex = DefaultClass();
    wcex->lpfnWndProc = MsgProc;
    wcex->lpszClassName = szTouchClass;
    if (!RegisterClassExW(wcex)) ErrorAndExit(L"Error registering window class. [%d]", GetLastError());
    delete wcex;
    wcex = DefaultClass();
    wcex->lpfnWndProc = QuitProc;
    wcex->lpszClassName = szDrawingClass;
    if (!RegisterClassExW(wcex)) ErrorAndExit(L"Error registering window class. [%d]", GetLastError());
    delete wcex;

    return 0;
}
int status = WindowsInit();

InputAndCursor::~InputAndCursor() {
    DeleteObject(hdc);
    CloseWindow(hwnd);
}

void InputAndCursor::Start() {
    if (!hwnd) throw "windows is nullptr";
    if (!callback) throw "callback is nullptr";
    if (!RegisterPointerInputTarget(hwnd, PT_TOUCH)) {
        LastIdThrow(L"[RegisterPointerInputTarget] %d.", GetLastError());
    }
    input = this;
    SetTimer(hwnd, 1, 1000, NULL);
    ShowCursor(FALSE);
    MSG msg;
    TouchPoint tp;
    mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, 0, 0, 0, 0);
    while (GetMessage(&msg, nullptr, 0, 0)) {
        switch (msg.message) {
        case WM_POINTERENTER:
        case WM_POINTERLEAVE:
            continue;
        case WM_POINTERDOWN:
        case WM_POINTERUPDATE:
        case WM_POINTERUP:
            tp = msg;
            callback->OnPoint(tp);
            if (tp.x < 10 && tp.y < 10) {
                UnregisterPointerInputTarget(hwnd, PT_TOUCH);
                exit(0);
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void InputAndCursor::Move(INT16 x, INT16 y) {
    mouse_event(MOUSEEVENTF_MOVE, x, y, 0, 0);
    CURSORINFO hcur;
    hcur.cbSize = sizeof(hcur);
    GetCursorInfo(&hcur);
    HDC mdc = CreateCompatibleDC(hdc);
    HBITMAP hbitmap = CreateCompatibleBitmap(hdc, ps.cx, ps.cy);
    HGDIOBJ hobj = SelectObject(mdc, hbitmap);
    hcur.ptScreenPos.x = hcur.ptScreenPos.x - 10;
    hcur.ptScreenPos.y = hcur.ptScreenPos.y - 10;
    if (hcur.hCursor) cursor = hcur.hCursor;
    DrawIconEx(mdc, 0, 0, cursor, 0, 0, 0, NULL, DI_IMAGE);
    UpdateLayeredWindow(hwnd, hdc, &hcur.ptScreenPos, &ps, mdc, &psrc, 0, &blend, ULW_OPAQUE);// ULW_ALPHA);
    SelectObject(hdc, hobj);
    DeleteObject(mdc);
    DeleteObject(hbitmap);
}

void InputAndCursor::Pos(INT16 x, INT16 y) {
    INT16 width = GetSystemMetrics(SM_CXSCREEN);
    INT16 height = GetSystemMetrics(SM_CYSCREEN);
    x = x - 100;
    y = y - 100;
    mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x * 0xFFFF / width, y * 0xFFFF / height, 0, 0);
    CURSORINFO hcur;
    hcur.cbSize = sizeof(hcur);
    GetCursorInfo(&hcur);
    HDC mdc = CreateCompatibleDC(hdc);
    HBITMAP hbitmap = CreateCompatibleBitmap(hdc, ps.cx, ps.cy);
    HGDIOBJ hobj = SelectObject(mdc, hbitmap);
    if (hcur.hCursor) cursor = hcur.hCursor;
    DrawIconEx(mdc, 0, 0, cursor, 0, 0, 0, NULL, DI_IMAGE);
    UpdateLayeredWindow(hwnd, hdc, &hcur.ptScreenPos, &ps, mdc, &psrc, 0, &blend, ULW_ALPHA);
    SelectObject(hdc, hobj);
    DeleteObject(mdc);
    DeleteObject(hbitmap);
}

std::mutex iac_mutex;
void InputAndCursor::Initialization() {
    iac_mutex.lock();
    if (hwnd) { iac_mutex.unlock();return; };
    hwnd = MsgWindow();
    iac_mutex.unlock();
    if (!hwnd) throw "cursor create error.";
    ShowWindow(hwnd, 1);
    UpdateWindow(hwnd);
    hdc = GetDC(hwnd);
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
}

class DebugDataFile {
protected:
    size_t len = 0;
public:
    size_t Length() { return len; }
    TouchPoint* Read() {
        std::ifstream in("C:\\Users\\zr\\Downloads\\1\\data.txt", std::ios::in | std::ios::binary | std::ios::ate);
        if (!in.is_open()) {
            std::cout << "open read error" << std::endl;
            return nullptr;
        }
        size_t length = in.tellg();
        in.seekg(0, std::ios::beg);
        char* buf = new char[length];
        in.read(buf, length);
        TouchPoint* r = (TouchPoint*)buf;
        len = length / sizeof(TouchPoint);
        in.close();
        return r;
    }
    void Write(std::vector<TouchPoint*> tp) {
        if (tp.size() == 0) return;
        std::ofstream out("C:\\Users\\zr\\Downloads\\1\\data.txt", std::ios::binary);
        if (!out.is_open()) {
            std::cout << "open write error" << std::endl;
            return;
        }
        TouchPoint** t = tp.data();
        for (size_t i = 0; i < tp.size(); i++) {
            char* ct = (char*)t[i];
            for (size_t n = 0; n < sizeof(TouchPoint); n++) {
                out << ct[n];
            }
        }
        out.close();
    }
};

void DebugInput::Start() {
    if (!hwnd) throw "windows is nullptr";
    if (!callback) throw "callback is nullptr";
    input = this;
    TouchPoint tp, * tp0;
    int id = 0;
    uint16 sta = 0, first = 0;
    std::vector<TouchPoint*> tps;
    DebugDataFile ddf;
    while (true) {
        std::cout << "function(0.run 1.write 2.read 3.input): ";
        std::cin >> id;
        if (id == 0) {
            std::cout << "run: " << std::endl;
            for (size_t i = 0; i < tps.size(); i++) {
                tp0 = tps[i];
                std::cout << (int)tp0->sta << "," << tp0->id << "," << tp0->x << ","
                    << tp0->y << "," << tp0->time << "," << (int)tp0->first << std::endl;
                callback->OnPoint(*tp0);
                Sleep(500);
            }
            continue;
        }
        else if (id == 1) {
            ddf.Write(tps);
            continue;
        }
        else if (id == 2) {
            TouchPoint* tpt = ddf.Read();
            tps.clear();
            for (size_t i = 0; i < ddf.Length(); i++) {
                tps.push_back(&tpt[i]);
            }
            continue;
        }
        std::cout << "atgs(sta, id, x, y, time, first): ";
        std::cin >> sta >> tp.id >> tp.x >> tp.y >> tp.time >> first;
        tp.sta = (byte)sta, tp.first = (byte)first;
        if (tp.sta == 1) continue;
        callback->OnPoint(tp);
        TouchPoint* t = new TouchPoint;
        memcpy(t, &tp, sizeof(TouchPoint));
        tps.push_back(t);
    }
}

void DebugDev::Start() {
    if (!hwnd) throw "windows is nullptr";
    if (!callback) throw "callback is nullptr";
    if (!RegisterPointerInputTarget(hwnd, PT_TOUCH)) {
        LastIdThrow(L"[RegisterPointerInputTarget] %d.", GetLastError());
    }
    input = this;
    SetTimer(hwnd, 1, 1000, NULL);
    ShowCursor(FALSE);
    MSG msg;
    TouchPoint tp;
    mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, 0, 0, 0, 0);
    while (GetMessage(&msg, nullptr, 0, 0)) {
        switch (msg.message) {
        case WM_POINTERENTER:
        case WM_POINTERLEAVE:
            continue;
        case WM_POINTERDOWN:
        case WM_POINTERUPDATE:
        case WM_POINTERUP:
            tp = msg;
            callback->OnPoint(tp);
            if (tp.x < 10 && tp.y < 10) {
                UnregisterPointerInputTarget(hwnd, PT_TOUCH);
                exit(0);
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
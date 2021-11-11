#include "widget.h"
#include <iostream>

Widget::Widget() {
    Size(100, 100);
}

Widget::~Widget() {
    CloseWindow(hwnd);
}

void Widget::Initialization() {
    CreateThread(NULL, 0, Msgloop, this, 0, NULL);
    while (!hwnd) Sleep(0);
}

void Widget::Show(byte v) {
    ShowWindow(hwnd, v);
    UpdateWindow(hwnd);
}

DWORD __stdcall Widget::Msgloop(LPVOID param) {
    Widget* _this = (Widget*)param;
    _this->hwnd = DrawingWindow();
    if (_this->hwnd == 0) throw "Drawing Windows Error.";
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void Widget::UpdateLayered() {
    IGraphics* ddg = Handler()->Graphics();
    HDC hdc = ddg->GetHDC();
    HDC mdc = ddg->GetCompatibleDC();
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    POINT ptSrc = { 0, 0 };
    UpdateLayeredWindow(hwnd, hdc, &Point(), &Size(), mdc, &ptSrc, 0, &blend, ULW_ALPHA);
}

/*void Hotspot::OnDown(InjectPoint*) {
    status = 1;
}

void Hotspot::OnUp(InjectPoint*) {
    status = 0;
}*/

void HotspotGroup::Initialization() {
    Widget::Initialization();
    for (size_t i = 0; i < button.size(); i++) {
        button[i]->Initialization();
    }
}

void HotspotGroup::Point(FLOAT x, FLOAT y) {
    Widget::Point(x, y);
    size_t len = button.size();
    ::Hotspot** b = button.data(), * pb = nullptr;
    for (size_t i = 0; i < len; i++) {
        pb = b[i];
        pb->X(x + pb->Rx()), pb->Y(y + pb->Ry());
    }
    if (Handler()) Handler()->OnPointChange();
}

void HotspotGroup::X(FLOAT x) {
    Widget::X(x);
    size_t len = button.size();
    ::Hotspot** b = button.data(), * pb = nullptr;
    for (size_t i = 0; i < len; i++) {
        pb = b[i];
        pb->X(x + pb->Rx());
    }
}

void HotspotGroup::Y(FLOAT y) {
    Widget::Y(y);
    size_t len = button.size();
    ::Hotspot** b = button.data(), * pb = nullptr;
    for (size_t i = 0; i < len; i++) {
        pb = b[i];
        pb->Y(y + pb->Ry());
    }
}

void HotspotGroup::Hotspot(INT16 x, INT16 y, ::Hotspot* btn) {
    btn->Rx(x), btn->Ry(y), btn->X(btn->Rx() + X()), btn->Y(btn->Ry() + Y());
    if (btn->Right() > Width()) Width(btn->Right() - X());
    if (btn->Bottom() > Height()) Height(btn->Bottom() - Y());
    btn->Parent(this);
    button.push_back(btn);
    // btn->Initialization();
}

void HotspotGroup::OnPaint() {
    IGraphics* ddg = Handler()->Graphics();
    ddg->BeginDraw(Width(), Height());
    size_t len = button.size();
    ::Hotspot** b = button.data();
    for (size_t i = 0; i < len; i++) {
        b[i]->OnPaint(ddg);
    }
    ddg->EndDraw();
    UpdateLayered();
    ddg->Clear();
}

void HotspotGroup::Scale(FLOAT s) {
    size_t len = button.size();
    ::Hotspot** b = button.data(), * pb = nullptr;
    for (size_t i = 0; i < len; i++) {
        pb = b[i];
        pb->Rx(pb->Rx() * s), pb->Ry(pb->Ry() * s);
        pb->X(X() + pb->Rx()), pb->Y(Y() + pb->Ry());
        pb->Width(pb->Width() * s), pb->Height(pb->Height() * s);
        if (pb->Right() > Right()) Width(pb->Right() - X());
        if (pb->Bottom() > Bottom()) Height(pb->Bottom() - Y());
    }
}

void Factory::Default() {
    if (!ddg) ddg = new D2D1Graphics();
    if (input) {
        input->Callback(this);
        input->Initialization();
    }
    if (cursor) {
        cursor->Initialization();
    }
    if (!(!cursor || !input)) {
        return;
    }
    InputAndCursor* iac = new InputAndCursor();
    if (!cursor) cursor = (ICursor*)iac;
    if (!input) {
        input = (IInput*)iac;
        input->Callback(this);
        input->Initialization();
    }
}

void Factory::OnTimer() {
    //std::cout << "timer" << std::endl;
}

void Factory::OnPoint(TouchPoint& point) {
    InjectData& ijd = Behavior(&point);
    for (size_t i = 0; i < ijd.Length(); i++) {
        switch (ijd[i].Type)
        {
        case InjectType::None:
            break;
        case InjectType::LeftDown:
            #ifdef _DEBUG
            std::cout << "leftdown" << std::endl;
            #endif
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            break;
        case InjectType::LeftUp:
            #ifdef _DEBUG
            std::cout << "leftup" << std::endl;
            #endif
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            break;
        case InjectType::RightDown:
            #ifdef _DEBUG
            std::cout << "rightdown" << std::endl;
            #endif
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
            break;
        case InjectType::RightUp:
            #ifdef _DEBUG
            std::cout << "rightup" << std::endl;
            #endif
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
            break;
        case InjectType::Move:
            cursor->Move((int16)(ijd[i].Point.ax * speed), (int16)(ijd[i].Point.ay * speed));
            break;
        case InjectType::To:
            break;
        case InjectType::KeyDown:
            #ifdef _DEBUG
            std::cout << "keydown" << std::endl;
            #endif
            ijd[i].Hot->OnDown(&ijd[i].Point);
            break;
        case InjectType::KeyUp:
            #ifdef _DEBUG
            std::cout << "keyup" << std::endl;
            #endif
            ijd[i].Hot->OnUp(&ijd[i].Point);
            break;
        case InjectType::KeyHold:
            #ifdef _DEBUG
            std::cout << "keyhold" << std::endl;
            #endif
            ijd[i].Hot->OnHold(&ijd[i].Point);
            break;
        case InjectType::KeyMove:
            #ifdef _DEBUG
            std::cout << "keymove" << std::endl;
            #endif
            ijd[i].Hot->OnMove(&ijd[i].Point);
            break;
        default:
            break;
        }
    }
}

WidgetHotspot::WidgetHotspot() :name(L"") {}

WidgetHotspot::WidgetHotspot(const char* n) {
    name = CharToWchar(n);
}

WidgetHotspot::~WidgetHotspot() {
    delete name;
}

void WidgetHotspot::Initialization() {
    Widget::Initialization();
    Handler()->Hotspot("key", this);
}

void WidgetHotspot::OnPaint() {
    IGraphics* ddg = Handler()->Graphics();
    ddg->BeginDraw(Width(), Height());
    if (status) {
        ddg->Brush(1.0f, 1.0f, 1.0f, .5f);
        ddg->FillRectangle(0, 0, Width(), Height());
        ddg->Brush(.0f, .0f, .0f, .5f);
        ddg->FillRectangle(0, 0, Width(), Height());
        ddg->Brush(1.0f, 1.0f, 1.0f, .85f);
        ddg->DrawTextW(name, 0, 0, Width(), Height());
    }
    else {
        ddg->Brush(.0f, .0f, .0f, .5f);
        ddg->FillRectangle(0, 0, Width(), Height());
        ddg->Brush(1.0f, 1.0f, 1.0f, .5f);
        ddg->FillRectangle(0, 0, Width(), Height());
        ddg->Brush(.0f, .0f, .0f, .85f);
        ddg->DrawTextW(name, 0, 0, Width(), Height());
    }
    ddg->EndDraw();
    UpdateLayered();
    ddg->Clear();
}
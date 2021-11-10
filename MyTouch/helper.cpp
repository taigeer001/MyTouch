#include "helper.h"

void ErrorAndExit(const WCHAR* text, DWORD id) {
    if (id != -1) {
        WCHAR buf[70];
        wsprintf(buf, L"The function cannot run properly, Error %d", id);
        MessageBox(0, buf, TEXT("Error"), MB_TOPMOST | MB_ICONERROR | MB_SYSTEMMODAL);
    }
    else {
        MessageBox(0, text, TEXT("Error"), MB_TOPMOST | MB_ICONERROR | MB_SYSTEMMODAL);
    }
    exit(0);
}

void LastIdThrow(const WCHAR* text, DWORD id) {
    if (id != -1) {
        WCHAR buf[70];
        wsprintf(buf, L"The function cannot run properly, Error %d", id);
        throw buf;
    }
    else {
        throw text;
    }
}

const wchar_t* CharToWchar(const char* ch) {
    size_t pl = 0;
    const size_t len = strlen(ch) + 1;
    wchar_t* wch = new wchar_t[len];
    mbstowcs_s(&pl, wch, len, ch, _TRUNCATE);
    return wch;
}

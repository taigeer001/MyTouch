#include "type.h"

TouchPoint& TouchPoint::operator=(MSG& msg) {
    static union { long long ns;FILETIME ft; } nstime;
    GetSystemTimeAsFileTime(&nstime.ft);
    sta = IS_POINTER_NEW_WPARAM(msg.wParam) | (IS_POINTER_INRANGE_WPARAM(msg.wParam) << 1);
    first = IS_POINTER_PRIMARY_WPARAM(msg.wParam);
    id = GET_POINTERID_WPARAM(msg.wParam);
    x = LOWORD(msg.lParam);
    y = HIWORD(msg.lParam);
    time = nstime.ns;
    return *this;
}

void DpiRectangle::OnDisplayChange(UINT sxt, UINT syt, UINT nsxt, UINT nsyt) {
    FLOAT dpi = ((FLOAT)sxt + (FLOAT)syt) / (nsxt + nsyt), nw = width * dpi, nh = height * dpi;
    switch (Config.left | (Config.right << 1)) {
    case 3:x = x / (sxt - width) * (nsxt - nw);width = nw;break;
    case 2: x = nsxt - (sxt - Right()) * dpi - nw, width = nw;break;
    case 0:case 1: x = x * dpi, width = nw;break;
    }
    switch (Config.top | (Config.bottom << 1)) {
    case 3:y = y / (syt - height) * (nsyt - nh);height = nh;break;
    case 2: y = nsyt - (syt - Bottom()) * dpi - nh, height = nh;break;
    case 0:case 1: y = y * dpi, height = nh;break;
    }
};

class :public InjectData {
public:
    inline size_t Length() {
        return 0;
    }
} injectdatanull;
extern InjectData& InjectDataNull = injectdatanull;

void IInject::Hotspot(IHotspot* hot) {
    if (hotspot) {
        IHotspot** t = new IHotspot * [length];
        memcpy(t, hotspot, length * sizeof(IHotspot*));
        SafeDeletes(hotspot);
        hotspot = new IHotspot * [length + 1];
        memcpy(hotspot, t, length * sizeof(IHotspot*));
        SafeDeletes(t);
    }
    else {
        hotspot = new IHotspot * [1];
    }
    hotspot[length++] = hot;
}

void IHandler::Inject(IInject* ib) {
    obj.push_back(ib);
    ib->Initialization();
}

InjectData* IHandler::OpenInject(PointInfo* pi, TouchPoint* tp) {
    if (pi->id == 10) return nullptr;
    size_t s = obj.size();
    IInject** l = obj.data(), * t = nullptr;
    for (size_t i = 0; i < s; i++) {
        t = l[i];
        InjectData* d = t->StartInput(&table, pi, tp);
        if (d) {
            pi->ok = 1, pi->ijb = t, table.idt[pi->id] = pi;
            table.Occupy(pi->id);
            return d;
        }
    }
    return nullptr;
}

InjectData& IHandler::Behavior(TouchPoint* tp) {
    InjectData* ijd = nullptr;
    PointInfo* pi = &table.info[tp->id];
    switch (tp->sta) {
    case 2:
        if (pi->ok == 1) ijd = pi->ijb->Input(&table, pi, tp);
        break;
    case 3:
        pi->id = table.Id();
        ijd = OpenInject(pi, tp);
        break;
    case 0:
        if (pi->ok == 1) {
            ijd = pi->ijb->CloseInput(&table, pi, tp);
            pi->ok = 0;
        }
        table.Release(pi->id);
        break;
    }
    return ijd == nullptr ? InjectDataNull : *ijd;
}

void IHandler::Hotspot(const char* n, IHotspot* hp) {
    for (size_t i = 0; i < obj.size(); i++) {
        if (!Equal(obj[i]->TypeName(), n)) continue;
        obj[i]->Hotspot(hp);
    }
}


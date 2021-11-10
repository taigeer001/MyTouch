#include "handler.h"
#include <iostream>

InjectData* InjectBlock::StartInput(PointTable*, PointInfo*, TouchPoint* tp) {
    for (size_t i = 0; i < length; i++) {
        if (hotspot[i]->Enable() && hotspot[i]->Inside(tp->x, tp->y)) {
            return &InjectDataNull;
        }
    }
    return nullptr;
}

void InjectBlock::Hotspot(IHotspot* hot) {
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

InjectData* InjectKey::StartInput(PointTable* pt, PointInfo* pi, TouchPoint* tp) {
    cache.Length(0);
    IHotspot* ihp = nullptr;
    for (size_t i = 0; i < length; i++) {
        if (hotspot[i]->Enable() && hotspot[i]->Inside(tp->x, tp->y)) {
            cache.Append(InjectType::KeyDown, hotspot[i], tp);
            table[pi->id].x = tp->x, table[pi->id].y = tp->y;
            table[pi->id].time = tp->time;
            table[pi->id].hotspot = hotspot[i];
            return &cache;
        }
    }
    return nullptr;
}

InjectData* InjectKey::Input(PointTable*, PointInfo* pi, TouchPoint* tp) {
    cache.Length(0);
    INT16 xt = table[pi->id].x - tp->x, yt = table[pi->id].y - tp->y;
    if (xt != 0 || yt != 0) {
        cache.Append(InjectType::KeyMove, table[pi->id].hotspot, tp, xt, yt);
        table[pi->id].x = tp->x, table[pi->id].y = tp->y;
    }
    if (tp->time - table[pi->id].time >= 1000000) {
        cache.Append(InjectType::KeyHold, table[pi->id].hotspot, tp);
        table[pi->id].time = tp->time;
    }
    return &cache;
}

InjectData* InjectKey::CloseInput(PointTable* pt, PointInfo* pi, TouchPoint* tp) {
    cache.Length(0);
    cache.Append(InjectType::KeyUp, table[pi->id].hotspot, tp);
    return &cache;
}

void InjectKey::Hotspot(IHotspot* hot) {
    InjectBlock::Hotspot(hot);
    cache.Size(length);
}

InjectData* InjectJoystick::StartInput(PointTable* pt, PointInfo* pi, TouchPoint* tp) {
    cache.Length(0);
    IHotspot* ihp = nullptr;
    for (size_t i = 0; i < length; i++) {
        if (hotspot[i]->Enable() && hotspot[i]->Inside(tp->x, tp->y)) {
            cache.Append(InjectType::KeyDown, hotspot[i], tp);
            table[pi->id].x = tp->x, table[pi->id].y = tp->y;
            table[pi->id].time = tp->time;
            table[pi->id].hotspot = hotspot[i];
            return &cache;
        }
    }
    return nullptr;
}

InjectMouse::InjectMouse() {
    Clear();
    cache.Size(2);
    sta = new Status[10];
    zero = &sta[0];
    //aid = new UINT16[10];
    //code = new BYTE[10];
}

InjectMouse::~InjectMouse() {
    delete[] sta;//aid, code;
}

InjectData* InjectMouse::StartInput(PointTable* pt, PointInfo* pi, TouchPoint* tp) {
    cache.Length(0);
    uint16 id = Id();
    sta[pi->id].id = id;
    Occupy(id);
    sta[pi->id].x = tp->x, sta[pi->id].y = tp->y;
    if (id == 0) {
        zero = &sta[pi->id];
        return &cache;
    }
    if (PointType(tp)) {
        sta[pi->id].code = 2;
        cache.Append(InjectType::RightDown, tp);
    }
    else {
        sta[pi->id].code = 1;
        cache.Append(InjectType::LeftDown, tp);
    }
    return &cache;
}

InjectData* InjectMouse::Input(PointTable*, PointInfo* pi, TouchPoint* tp) {
    cache.Length(0);
    UINT16 id = sta[pi->id].id;
    if (id != 0) return &cache;

    if (tp->x != zero->x || tp->y != zero->y) {
        cache.Append(InjectType::Move, tp, tp->x - zero->x, tp->y - zero->y);
        zero->x = tp->x, zero->y = tp->y;
    }

    return &cache;
}

InjectData* InjectMouse::CloseInput(PointTable* pt, PointInfo* pi, TouchPoint* tp) {
    cache.Length(0);
    UINT16 id = sta[pi->id].id;
    if ((id == 0) && (tp->x != zero->x || tp->y != zero->y)) {
        cache.Append(InjectType::Move, tp, tp->x - zero->x, tp->y - zero->y);
        //zero->x = tp->x, zero->y = tp->y;
        return &cache;
    }
    if (sta[pi->id].code == 1) {
        cache.Append(InjectType::LeftUp, tp);
    }
    else if (sta[pi->id].code == 2) {
        cache.Append(InjectType::RightUp, tp);
    }
    Release(id);
    return &cache;
}

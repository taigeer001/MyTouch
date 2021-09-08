#include "Gesture.h"
#include <iostream>
using namespace std;

int Gesture::GetPoint(UINT16 id)
{
    for (size_t i = 0; i < 10; i++)
    {
        TouchPoint tp = point[i];
        if (tp.Id == id) {
            return i;
        }
    }
    return -1;
}

TouchNode Gesture::NewTouchNode(int i, TouchData* d, Touch* t)
{
    /*
    struct TouchNode
    {
        BYTE Sta;
        BYTE Hold;
        BYTE TimeType;
        BYTE Oth;
        BYTE Zero;
        BYTE Zero1;
        BYTE Zero2;
    };
    */
    return { t->Sta, 0, 0, 0, 0, 0, 0, 0 };
}

TouchPoint Gesture::NewTouchPoint(int i, TouchData* d, Touch *t)
{
    TouchNode tn = NewTouchNode(i, d, t);
    node[i] = tn;
    return {i, CalcHash(&tn), t->Id, t->X, t->Y, d->Time};
}

TouchPoint* Gesture::PushPoint(TouchData* td, Touch* t)
{
    for (size_t i = 0; i < 10; i++)
    {
        TouchPoint tp = point[i];
        if (tp.Idx == -1) {
            point[i] = NewTouchPoint(i, td, t);
            return  &point[i];
        }
    }
    return NULL;
}

void Gesture::ClearIdx(TouchData* td, Touch* t)
{
    for (size_t i = 0; i < td->Length; i++)
    {

        point[i].Idx = -1;
    }
}

BYTE Gesture::CalcTimeType(UINT32 t)
{
    if (t >= 1000) {
        return 0x3;
    }else if (t >= 500) {
        return 0x1;
    }
    return 0;
}

BYTE Gesture::CalcHold(Touch* t, TouchPoint* tp)
{
    return t->X != tp->X || t->Y != tp->Y ? 1 : 0;
}

UINT64 Gesture::CalcHash(TouchNode *n)
{
    UINT64* h = (UINT64*)n;
    return *h;
}

ActionType Gesture::First(TouchPoint* tp, Touch* t)
{
    UINT64 h = tp->Hash;
    TouchNode tn = { TOUCHDOWN, 1, 3 };
    UINT64 a = CalcHash(&tn);
    tn = { TOUCHUP, 0, 0 };
    UINT64 b = CalcHash(&tn);
    if ((h & 0x0000000000FFFFFF) == a) {
        tp->X = t->X;
        tp->Y = t->Y;
        return Move;
    }
    else if((h & 0x00000000000200FF) == b) 
    {
        return To;
    }
    return None;
}

ActionType Gesture::Second(TouchPoint* tp, Touch* t)
{
    UINT64 h = tp->Hash;
    TouchNode tn = { TOUCHDOWN,0,1,0 };
    UINT64 a = CalcHash(&tn);
    tn = { TOUCHUP,0,1,1 };
    UINT64 b = CalcHash(&tn);
    tn = { TOUCHDOWN, 1, 0,0 };
    UINT64 c = CalcHash(&tn);
    tn = { TOUCHUP, 0, 0, 2 };
    UINT64 d = CalcHash(&tn);
    if ((h & 0x00000000FFFFFFFF) == c && (t->Y - tp->Y < -50)) {
        node[tp->Idx].Oth = 2;
        return RightDown;
    }
    else if ((h & 0x00000000FF0100FF) == a) {
        node[tp->Idx].Oth = 1;
        return LeftDown;
    }
    else if ((h & 0x00000000FF0100FF) == b)
    {
        node[tp->Idx].Oth = 0;
        return LeftUp;
    }
    else if ((h & 0x00000000FF0000FF) == d)
    {
        node[tp->Idx].Oth = 0;
        return RightUp;
    }
    return None;
}

ActionType Gesture::Other(TouchPoint* tp, Touch* t)
{
    return None;
}

Gesture::Gesture()
{
    for (size_t i = 0; i < 10; i++)
    {
        point[i].Idx = -1;
    }
}

Action Gesture::Action(TouchData* ptd)
{

    //ClearPoint();

    Touch* t;
    TouchPoint* tp;
    TouchData td = *ptd;
    int n = 0;
    for (size_t i = 0; i < td.Length; i++) {
        t = &td[i];
        n = GetPoint(t->Id);
        if (n == -1) {
            office[i] = PushPoint(&td, t);
        }
        else {
            TouchNode* tn = &node[n];
            tp = &point[n];
            tn->Sta = t->Sta;
            tn->TimeType = CalcTimeType(td.Time - tp->Time);
            tn->Hold = CalcHold(t, tp);
            tp->Hash = CalcHash(tn);
            office[i] = tp;
        }
    }
    ActionType at = None;
    for (size_t i = 0; i < td.Length; i++)
    {
        tp = office[i];
        switch (tp->Idx)
        {
        case 0:
            at = First(tp, &td[i]);
            if (at != None) {
                goto ACTION_END_CODE;
            }
            break;
        case 1:
            at = Second(tp, &td[i]);
            if (at != None) {
                goto ACTION_END_CODE;
            }
            break;
        }
    }

    ACTION_END_CODE:
    for (size_t i = 0; i < td.Length; i++)
    {
        if (td[i].Sta == TOUCHUP) {
            office[i]->Idx = -1;
        }
    }
    //cout << "size:" << td[0].Y - office[0]->Y << endl;
    return { at, NULL };
}
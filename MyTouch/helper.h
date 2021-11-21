#pragma once

#include <Windows.h>

#define uint unsigned int
#define int32 INT32
#define uint16 UINT16
#define int16 INT16
#define byte BYTE
#define float32 FLOAT

#define EVENT_FID 99

#ifdef _MSC_VER_
#define forceinline __forceinline
#elif defined __GNUC__
#define forceinline __inline__ __attribute__((always_inline))
#else
#define forceinline inline
#endif

#define Equal(a,b)              (_stricmp((a),(b))==0)
#define EQUAL(a,b)              (strcmp((a),(b))==0)

#define Getbit(v,n) ((v>>n)&0x1)
#define GetNotbit(v,n) (v&(~(0x1<<n)))
#define Setbit(v, n) (v|(0x1<<n))
#define Clearbit(v, n) (v&(~(0x1<<n)))
#define Set4bit(v0,v1,n) ((v0&(~(0xF<<n)))|((v1&0xF)<<n))
#define SafeRelease(P) if(P){P->Release() ; P = NULL ;}
#define SafeDeletes(V) if (V) { delete[] V; V = nullptr; }

void ErrorAndExit(const WCHAR*, DWORD id = -1);

void LastIdThrow(const WCHAR*, DWORD id = -1);

const wchar_t* CharToWchar(const char*);

template<typename T> T** Extension(T**, size_t);

template<typename T>
inline T** Extension(T** data, size_t length) {
	if (data) {
		T** t = new T * [length];
		memcpy(t, data, length * sizeof(T*));
		SafeDeletes(data);
		data = new T * [length + 1];
		memcpy(data, t, length * sizeof(T*));
		SafeDeletes(t);
	}
	else {
		data = new T * [1];
	}
	return data;
};


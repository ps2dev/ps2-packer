#ifndef __COMPAT_WINDOWS_H
#define __COMPAT_WINDOWS_H

#include "7zTypes.h"

/* See p7zip's CPP/include_windows/windows.h */
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#undef BOOL
typedef int BOOL;

typedef void *LPVOID;

/* See LZMA SDK's CPP/Common/MyWindows.h */
typedef Int32 INT32;
typedef INT32 LONG;   // LONG, ULONG and DWORD must be 32-bit

#define HRESULT LONG

/* See p7zip's CPP/7zip/Compress/Lzham/lzhamdecomp/lzham_platform.h */
#define GCC_VERSION (__GNUC__ * 10000                 \
                      + __GNUC_MINOR__ * 100           \
                      + __GNUC_PATCHLEVEL__)

#if defined(GCC_VERSION) && GCC_VERSION >= 40700
// in GCC version >= 4.7.0 we can use the built-in __atomic operations
#define InterlockedIncrement(x) __atomic_add_fetch(x, 1, __ATOMIC_SEQ_CST)
#elif defined(GCC_VERSION) && GCC_VERSION >= 40100
// in GCC version >= 4.1.0 we can use the __sync built-in atomic operations
#define InterlockedIncrement(x) __sync_add_and_fetch(x, 1)
#else
#define InterlockedIncrement(x) (*x += 1)
#endif

#endif

#ifndef _KOS_COMPILER_H_
#define _KOS_COMPILER_H_

#include "stdio.h"

#if defined(__GNUC__)

#define __ASM__ __asm
#define __VOLATILE__ volatile

#ifndef __always_inline
#define __always_inline inline
#endif
#define __static_inline__ static inline

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define __UNUSED__ __attribute__((__unused__))
#define __USED__ __attribute__((__used__))
#define __PACKED__ __attribute__((packed))
#define __ALIGNED__(x) __attribute__((aligned(x)))
#define __PURE__ __attribute__((__pure__))
#define __CONST__ __attribute__((__const__))
#define __NO_RETURN__ __attribute__((__noreturn__))
#define __NAKED__ __attribute__((naked))
#define __WEAK__ __attribute__((weak))

#endif

#endif

#ifndef __SHERO_MACRO_H
#define __SHERO_MACRO_H

#if defined __GUNC__ || defined __llvm__
#   define SHERO_LICKLY(x)      __builtin_expect(!!(x), 1)
#   define SHERO_UNLICKLY(x)    __builtin_expect(!!(x), 0)
#else
#   define SHERO_LICKLY(x)     (x)
#   define SHERO_UNLICKLY(x)   (x)
#endif

#endif

#ifndef __HYPERWIN_UTILS_H_
#define __HYPERWINS_UTILS_H_

#include <ntddk.h>

#define hvPrint(format, ...) DbgPrint("HyperWin :: " \
									   format, \
									   __VA_ARGS__)
#endif
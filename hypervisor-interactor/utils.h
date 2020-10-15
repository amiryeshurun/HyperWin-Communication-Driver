#ifndef __HYPERWIN_UTILS_H_
#define __HYPERWINS_UTILS_H_

#include <ntddk.h>

#define hvPrint(format, ...) DbgPrint("HyperWin :: " \
									   format, \
									   __VA_ARGS__)
#define DEBUG_LEVEL_INFO 1
#define DEBUG_LEVEL_WARNING 2
#define DEBUG_LEVEL_DEBUG 3

#define DEBUG_LEVEL DEBUG_LEVEL_DEBUG

#endif
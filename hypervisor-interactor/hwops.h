#ifndef __HYPERWIN_OPERATIONS_H_
#define __HYPERWIN_OPERATIONS_H_

#include <wdm.h>

/* Communication block different operations */
typedef DWORD64 OPERATION, * POPERATION;

#define OPERATION_INIT 0x4857494e4954 // HWINIT ASCII

#endif
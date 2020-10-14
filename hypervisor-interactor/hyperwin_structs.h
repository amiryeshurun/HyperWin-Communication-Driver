#ifndef __HYPERWIN_STRUCTS_H_
#define __HYPERWIN_STRUCTS_H_

#include <ntddk.h>
#include "hyperwin_types.h"

typedef struct _HYPERWIN_MAIN_DATA
{
	DWORD64 PhysicalCommunicationBaseAddress;
	DWORD64 CommunicationBlockSize;
	BOOLEAN IsMapped;
	BYTE_PTR VirtualCommunicationBlockAddress;
	BOOLEAN IsInOperation;
	KSPIN_LOCK OperationSpinLock;
} HYPERWIN_MAIN_DATA, *PHYPERWIN_MAIN_DATA;

#endif
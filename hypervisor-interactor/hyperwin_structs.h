#ifndef __HYPERWIN_STRUCTS_H_
#define __HYPERWIN_STRUCTS_H_

#include "types.h"

typedef struct _HYPERWIN_MAIN_DATA
{
	QWORD PhysicalCommunicationBaseAddress;
	QWORD CommunicationBlockSize;
	BOOLEAN IsMapped;
	BYTE_PTR VirtualCommunicationBlockAddress;
	BOOLEAN IsInOperation;
	KSPIN_LOCK OperationSpinLock;
} HYPERWIN_MAIN_DATA, *PHYPERWIN_MAIN_DATA;

#endif
#ifndef __HYPERWIN_TYPES_H_
#define __HYPERWIN_TYPES_H_

#include <ntddk.h>

#define HWSTATUS DWORD64

typedef unsigned char BYTE, * BYTE_PTR;
typedef DWORD64* DWORD64_PTR;

typedef struct _HYPERWIN_MAIN_DATA
{
	DWORD64 PhysicalCommunicationBaseAddress;
	DWORD64 CommunicationBlockSize;
	DWORD64 CurrentOffsetInBlock;
	BOOLEAN IsMapped;
	BYTE_PTR VirtualCommunicationBlockAddress;
	KSPIN_LOCK OperationSpinLock;
} HYPERWIN_MAIN_DATA, *PHYPERWIN_MAIN_DATA;

#endif
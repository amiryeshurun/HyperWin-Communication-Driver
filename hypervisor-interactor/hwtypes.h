#ifndef __HYPERWIN_TYPES_H_
#define __HYPERWIN_TYPES_H_

#include <ntddk.h>

#define HWSTATUS DWORD64

typedef DWORD64 OPERATION, * POPERATION;
typedef unsigned char BYTE, * BYTE_PTR;
typedef DWORD64* DWORD64_PTR;

typedef struct _HYPERWIN_MAIN_DATA
{
	DWORD64 PhysicalWritePipe;
	BYTE_PTR VirtualWritePipe;
	DWORD64 WritePipeSize;
	DWORD64 PhysicalReadPipe;
	BYTE_PTR VirtualReadPipe;
	DWORD64 ReadPipeSize;
	DWORD64 CurrentWriteOffset;
	BOOLEAN IsMapped;
	KSPIN_LOCK WritePipeSpinlock;
} HYPERWIN_MAIN_DATA, *PHYPERWIN_MAIN_DATA;

#endif
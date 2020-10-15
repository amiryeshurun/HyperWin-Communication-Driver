#ifndef __HYPERVIN_VMMINTR_H_
#define __HYPERVIN_VMMINTR_H_

#include "hwtypes.h"

// 
// These values are agreed values used to interact with the hypervisor
//
#define VMCALL_COMMUNICATION_BLOCK 0x487970657257696e

extern HWSTATUS HyperWinVmCall(IN DWORD64 HyperwinRax);

#endif
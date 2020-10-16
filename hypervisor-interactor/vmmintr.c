#include "vmmintr.h"

HWSTATUS ComSendSignal(IN DWORD64 CommunicationOffset)
{
	return HyperWinVmCall(VMCALL_COMMUNICATION_BLOCK, CommunicationOffset);
}
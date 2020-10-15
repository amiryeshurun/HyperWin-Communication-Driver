#include "comblock.h"
#include "vmmintr.h"

HWSTATUS ComSendInitSignal(IN BYTE_PTR VirtualCommunicationAddress)
{
	HWSTATUS HwStatus = HYPERWIN_STATUS_SUCCUESS;
	PGENERIC_COM_STRUCT Args = (PGENERIC_COM_STRUCT)VirtualCommunicationAddress;
	Args->Operation = OPERATION_INIT;
	Args->ArgumentsUnion.InitArgs.IsMessageAvailable = TRUE;
	RtlCopyMemory(Args->ArgumentsUnion.InitArgs.Message, "INIT", 4);
	if (HyperWinVmCall(VMCALL_COMMUNICATION_BLOCK))
		HwStatus = HYPERWIN_INIT_FAILED;
	return HwStatus;
}
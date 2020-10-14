#include "driver_operations.h"
#include "hyperwin_structs.h"

NTSTATUS HyperWinCreate(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp)
{
	//
	// When a create call is being used, we need to make sure that the communication block
	// is mapped in to the kernel's virtual memory
	//
	PHYPERWIN_MAIN_DATA pData = (PHYPERWIN_MAIN_DATA)pDeviceObj->DeviceExtension;
	if (!(pData->IsMapped))
	{
		KIRQL OldIrql;
		KeAcquireSpinLock(&(pData->OperationSpinLock), &OldIrql);
		DWORD32 Values[4];
		__cpuidex(Values, 0x40020020, 0);
		// The hypervisor will store the result in EDX:EAX
		DWORD64 PhysicalAddress = ((DWORD64)Values[3] << 32) | (Values[0]);
		pData->PhysicalCommunicationBaseAddress = PhysicalAddress;
		// Map the memory to the kernel's virtual address
	}

}
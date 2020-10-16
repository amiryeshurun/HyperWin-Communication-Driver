#include "drvops.h"
#include "hwtypes.h"
#include "utils.h"
#include "x86_64.h"
#include "hwstatus.h"
#include "vmmintr.h"

NTSTATUS HyperWinCreate(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp)
{
	//
	// When a create call is being used, we need to make sure that the communication block
	// is mapped in to the kernel's virtual memory
	//
	UNREFERENCED_PARAMETER(Irp);
	PHYPERWIN_MAIN_DATA pData = (PHYPERWIN_MAIN_DATA)pDeviceObj->DeviceExtension;
	NTSTATUS NtStatus = STATUS_SUCCESS;
	if (!(pData->IsMapped))
	{
		KIRQL OldIrql;
		KeAcquireSpinLock(&(pData->OperationSpinLock), &OldIrql);
		int Values[4];
		__cpuidex(Values, 0x40020020, 0);
		//
		// The hypervisor will store the result in EDX:EAX
		//
#if DEBUG_LEVEL == 3
		hvPrint("CPUID result: %lx %lx %lx %lx\n", Values[0], Values[1], Values[2], Values[3]);
#endif
		DWORD64 PhysicalAddress = ((DWORD64)Values[3] << 32) | (Values[0]);
		hvPrint("Got physical address: %llx\n", PhysicalAddress);
		pData->PhysicalCommunicationBaseAddress = PhysicalAddress;
		pData->CommunicationBlockSize = LARGE_PAGE_SIZE;
		//
		// Map the memory to the kernel's virtual address
		//
		PHYSICAL_ADDRESS pa;
		pa.QuadPart = PhysicalAddress;
		pData->VirtualCommunicationBlockAddress = MmMapIoSpace(pa, LARGE_PAGE_SIZE, MmCached);
		pData->CurrentOffsetInBlock = 0;
		KeReleaseSpinLock(&(pData->OperationSpinLock), OldIrql);
	}

	return NtStatus;
}

NTSTATUS HyperWinDeviceIoControl(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp)
{

}
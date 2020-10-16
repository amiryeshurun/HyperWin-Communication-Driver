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

#define CTL_CODE_HW CTL_CODE(40000, 0x800, METHOD_BUFFERED, GENERIC_READ | GENERIC_WRITE)

NTSTATUS HyperWinDeviceIoControl(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp)
{
	NTSTATUS NtStatus = STATUS_SUCCESS;
	PIO_STACK_LOCATION pStackLocation = IoGetCurrentIrpStackLocation(Irp);
	PHYPERWIN_MAIN_DATA pData = (PHYPERWIN_MAIN_DATA)pDeviceObj->DeviceExtension;
	PVOID SystemBuffer = Irp->AssociatedIrp.SystemBuffer;

	switch (pStackLocation->Parameters.DeviceIoControl.IoControlCode)
	{
		case CTL_CODE_HW:
		{
			hvPrint("Received ctl code: %xl\n", CTL_CODE_HW);
			KIRQL Irql;
			KeAcquireSpinLock(&(pData->OperationSpinLock), &Irql);
			if (pData->CurrentOffsetInBlock + pStackLocation->Parameters.DeviceIoControl.InputBufferLength <
				pData->CommunicationBlockSize)
				pData->CurrentOffsetInBlock += pStackLocation->Parameters.DeviceIoControl.InputBufferLength;
			else
				pData->CurrentOffsetInBlock = 0;
			RtlCopyMemory(pData->VirtualCommunicationBlockAddress + pData->CurrentOffsetInBlock, 
					SystemBuffer, pStackLocation->Parameters.DeviceIoControl.InputBufferLength);
			if(ComSendSignal(pData->CurrentOffsetInBlock) != HYPERWIN_STATUS_SUCCUESS)
			{
				NtStatus = STATUS_FAIL_FAST_EXCEPTION;
				goto DeviceIoControlExit;
			}
			//
			// HypeWin sent a response
			//
			if (*(DWORD64_PTR)(pData->VirtualCommunicationBlockAddress + pData->CurrentOffsetInBlock))
			{

			}
		}
	}

DeviceIoControlExit:
	Irp->IoStatus.Status = NtStatus;
	if(NT_SUCCESS(NtStatus))
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return NtStatus;
}

NTSTATUS HyperWinUnsupported(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObj);
	UNREFERENCED_PARAMETER(Irp);

	return STATUS_NOT_SUPPORTED;
}

NTSTATUS HyperWinClose(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObj);
	UNREFERENCED_PARAMETER(Irp);

	return STATUS_SUCCESS;
}
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
		KeAcquireSpinLock(&(pData->WritePipeSpinlock), &OldIrql);
		//
		// Double lock checking
		//
		if (pData->IsMapped)
			goto Mapped;
		pData->IsMapped = TRUE;
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
		pData->PhysicalWritePipe = PhysicalAddress;
		pData->WritePipeSize = LARGE_PAGE_SIZE;
		//
		// Map the memory to the kernel's virtual address
		//
		PHYSICAL_ADDRESS pa;
		pa.QuadPart = PhysicalAddress;
		pData->VirtualWritePipe = MmMapIoSpace(pa, LARGE_PAGE_SIZE, MmCached);
		pData->CurrentWriteOffset = 0;
		//
		// Get memory address for HyperWin responses
		//
		__cpuidex(Values, 0x40040040, 0);
#if DEBUG_LEVEL == 3
		hvPrint("CPUID result: %lx %lx %lx %lx\n", Values[0], Values[1], Values[2], Values[3]);
#endif
		PhysicalAddress = ((DWORD64)Values[3] << 32) | (Values[0]);
		hvPrint("Got physical address: %llx\n", PhysicalAddress);
		pData->PhysicalReadPipe = PhysicalAddress;
		pData->ReadPipeSize = LARGE_PAGE_SIZE;
		pa.QuadPart = PhysicalAddress;
		pData->VirtualReadPipe = MmMapIoSpace(pa, LARGE_PAGE_SIZE, MmCached);
		KeReleaseSpinLock(&(pData->WritePipeSpinlock), OldIrql);
	}
Mapped:
	return NtStatus;
}

#define CTL_CODE_HW CTL_CODE(40000, 0x800, METHOD_BUFFERED, GENERIC_READ | GENERIC_WRITE)

NTSTATUS HyperWinDeviceIoControl(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp)
{
	NTSTATUS NtStatus = STATUS_SUCCESS;
	PIO_STACK_LOCATION pStackLocation = IoGetCurrentIrpStackLocation(Irp);
	PHYPERWIN_MAIN_DATA pData = (PHYPERWIN_MAIN_DATA)pDeviceObj->DeviceExtension;
	PVOID SystemBuffer = Irp->AssociatedIrp.SystemBuffer;
	DWORD64 SavedWriteOffset, ReadOffset = 0, ReadLength;
	KIRQL Irql;
	HWSTATUS HwStatus;

	switch (pStackLocation->Parameters.DeviceIoControl.IoControlCode)
	{
		case CTL_CODE_HW:
		{
			hvPrint("Received ctl code: %xl\n", CTL_CODE_HW);
			KeAcquireSpinLock(&(pData->WritePipeSpinlock), &Irql);
			if (pData->CurrentWriteOffset + pStackLocation->Parameters.DeviceIoControl.InputBufferLength <
				pData->WritePipeSize)
				pData->CurrentWriteOffset += pStackLocation->Parameters.DeviceIoControl.InputBufferLength;
			else
				pData->CurrentWriteOffset = 0;
			SavedWriteOffset = pData->CurrentWriteOffset;
			KeReleaseSpinLock(&(pData->WritePipeSpinlock), Irql);

			RtlCopyMemory(pData->VirtualWritePipe + pData->CurrentWriteOffset, 
					SystemBuffer, pStackLocation->Parameters.DeviceIoControl.InputBufferLength);
			HwStatus = ComSendSignal(pData->CurrentWriteOffset);
			*(DWORD64_PTR)(SystemBuffer) = HwStatus;
			Irp->IoStatus.Information = sizeof(HWSTATUS);
			hvPrint("Operation status: %lld\n", HwStatus);
			if(HwStatus != HYPERWIN_STATUS_SUCCUESS)
				goto DeviceIoControlExit;
			//
			// HypeWin sent a response?
			//
			if ((ReadOffset = *(DWORD64_PTR)(pData->VirtualWritePipe + SavedWriteOffset + 
				sizeof(OPERATION))) != OPERATION_COMPLETED)
			{
				ReadLength = *(DWORD64_PTR)(pData->VirtualWritePipe + pData->CurrentWriteOffset 
					+ 2 * sizeof(DWORD64));
				RtlCopyMemory(pData->VirtualReadPipe + ReadOffset, (BYTE_PTR)SystemBuffer +
						sizeof(HWSTATUS), ReadLength);
				Irp->IoStatus.Information += ReadLength;
			}

			break;
		}
		default:
		{
			hvPrint("Unkown IOCTL code was sent: %xl\n", pStackLocation->Parameters.DeviceIoControl.IoControlCode);
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
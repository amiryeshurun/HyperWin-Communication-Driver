#include <ntddk.h>
#include "utils.h"
#include "hwtypes.h"
#include "drvops.h"
#include "x86_64.h"

UNICODE_STRING deviceName, dosDeviceName;

NTSTATUS DriverUnload(IN PDRIVER_OBJECT pDriverObj)
{
	hvPrint("Unloading driver...\n");
	//
	// Unmap memory used for communicating with HyperWin
	//
	if(((PHYPERWIN_MAIN_DATA)pDriverObj->DeviceObject->DeviceExtension)->IsMapped)
		MmUnmapIoSpace(((PHYPERWIN_MAIN_DATA)pDriverObj->DeviceObject->DeviceExtension)->VirtualCommunicationBlockAddress, 
			LARGE_PAGE_SIZE);
	IoDeleteSymbolicLink(&dosDeviceName);
	IoDeleteDevice(pDriverObj->DeviceObject);
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObj, IN PUNICODE_STRING RegPath)
{
	UNREFERENCED_PARAMETER(RegPath);
	hvPrint("Loading driver...\n");
	RtlInitUnicodeString(&deviceName, L"\\Device\\HyperWin");
	RtlInitUnicodeString(&dosDeviceName, L"\\DosDevice\\HyperWin");
	IoCreateSymbolicLink(&dosDeviceName, &deviceName);

	PDEVICE_OBJECT pDeviceObject;

	if (IoCreateDevice(pDriverObj, 
		sizeof(HYPERWIN_MAIN_DATA),
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&pDeviceObject) != STATUS_SUCCESS)
	{
		hvPrint("Could not create a device\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	pDeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);

	for (DWORD64 i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		pDriverObj->MajorFunction[i] = HyperWinUnsupported;
	pDriverObj->MajorFunction[IRP_MJ_CREATE] = HyperWinCreate;
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HyperWinDeviceIoControl;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = HyperWinClose;
	pDriverObj->DriverUnload = DriverUnload;

	PHYPERWIN_MAIN_DATA pMainData = (PHYPERWIN_MAIN_DATA)pDeviceObject->DeviceExtension;
	pMainData->CommunicationBlockSize = 0;
	pMainData->PhysicalCommunicationBaseAddress = 0;
	pMainData->IsMapped = FALSE;
	KeInitializeSpinLock(&(pMainData->OperationSpinLock));

	hvPrint("Driver loaded successfully\n");

	//
	// This section is here temporarly, for tests only
	//
	HyperWinCreate(pDeviceObject, NULL);

	return STATUS_SUCCESS;
}
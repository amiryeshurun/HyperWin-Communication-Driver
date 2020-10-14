#include <ntddk.h>
#include "utils.h"
#include "hyperwin_structs.h"

UNICODE_STRING deviceName, dosDeviceName;

NTSTATUS DriverUnload(IN PDRIVER_OBJECT pDriverObj)
{
	hvPrint("Unloading driver...\n");

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
	pDriverObj->DriverUnload = DriverUnload;
	PHYPERWIN_MAIN_DATA pMainData = (PHYPERWIN_MAIN_DATA)pDeviceObject->DeviceExtension;
	pMainData->CommunicationBlockSize = 0;
	pMainData->PhysicalCommunicationBaseAddress = 0;
	pMainData->IsMapped = FALSE;
	KeInitializeSpinLock(&(pMainData->OperationSpinLock));

	hvPrint("Driver loaded successfully\n");
	return STATUS_SUCCESS;
}
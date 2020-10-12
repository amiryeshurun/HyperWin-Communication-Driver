#include "driver_operations.h"
#include "hyperwin_structs.h"

NTSTATUS HyperWinCreate(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp)
{
	//
	// When a device is being created, we need to make sure that the communication blocked
	// is mapped in to the kernel's virtual memory
	//
}
#ifndef __HV_DRIVER_OPERATIONS_H_
#define __HV_DRIVER_OPERATIONS_H_

#include <ntddk.h>

NTSTATUS HyperWinCreate(IN PDEVICE_OBJECT pDeviceObj, IN PIRP Irp);

#endif
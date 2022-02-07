#pragma once

UNICODE_STRING ansi_to_unicode(const char* str)
{
	UNICODE_STRING unicode;
	ANSI_STRING ansi_str;

	RtlInitAnsiString(&ansi_str, str);
	RtlAnsiStringToUnicodeString(&unicode, &ansi_str, TRUE);

	return unicode;
}

PVOID get_kernel_proc_address(const char* system_routine_name)
{
	UNICODE_STRING name;
	ANSI_STRING ansi_str;

	RtlInitAnsiString(&ansi_str, system_routine_name);
	RtlAnsiStringToUnicodeString(&name, &ansi_str, TRUE);

	return MmGetSystemRoutineAddress(&name);
}

PVOID get_module_base(const char* module_name, ULONG* size_module)
{
	PLIST_ENTRY ps_loaded_module_list = PsLoadedModuleList;
	if (!ps_loaded_module_list)
		return (PVOID)NULL;

	UNICODE_STRING name = ansi_to_unicode(module_name);
	for (PLIST_ENTRY link = ps_loaded_module_list; link != ps_loaded_module_list->Blink; link = link->Flink)
	{
		PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(link, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);

		if (RtlEqualUnicodeString((PCUNICODE_STRING)&entry->BaseDllName, (PCUNICODE_STRING)&name, TRUE))
		{
			*size_module = entry->SizeOfImage;
			
			return (PVOID)entry->DllBase;
		}
	}

	return (PVOID)NULL;
}

NTSTATUS init_mouse(PMOUSE_OBJECT mouse_obj)
{
	PDRIVER_OBJECT class_driver_object = NULL;
	UNICODE_STRING class_string = ansi_to_unicode("\\Driver\\MouClass");
	const POBJECT_TYPE* io_driver_object_type = reinterpret_cast<POBJECT_TYPE*>(MmGetSystemRoutineAddress(&ansi_to_unicode("IoDriverObjectType")));

	NTSTATUS status = ObReferenceObjectByName(&class_string, OBJ_CASE_INSENSITIVE, NULL, 0, *io_driver_object_type, KernelMode, NULL, (PVOID*)&class_driver_object);
	if (!NT_SUCCESS(status))
		return status;

	PDRIVER_OBJECT hid_driver_object = NULL;
	UNICODE_STRING hid_string = ansi_to_unicode("\\Driver\\MouHID");

	status = ObReferenceObjectByName(&hid_string, OBJ_CASE_INSENSITIVE, NULL, 0, *io_driver_object_type, KernelMode, NULL, (PVOID*)&hid_driver_object);
	if (!NT_SUCCESS(status))
	{
		if (class_driver_object)
			ObfDereferenceObject(class_driver_object);

		return status;
	}

	PVOID class_driver_base = NULL;
	PDEVICE_OBJECT hid_device_object = hid_driver_object->DeviceObject;

	while (hid_device_object && !mouse_obj->service_callback)
	{
		PDEVICE_OBJECT class_device_object = class_driver_object->DeviceObject;
		while (class_device_object && !mouse_obj->service_callback)
		{
			if (!class_device_object->NextDevice && !mouse_obj->mouse_device)
				mouse_obj->mouse_device = class_device_object;

			PULONG_PTR device_extension = (PULONG_PTR)hid_device_object->DeviceExtension;
			ULONG_PTR device_ext_size = ((ULONG_PTR)hid_device_object->DeviceObjectExtension - (ULONG_PTR)hid_device_object->DeviceExtension) / 4;
			class_driver_base = class_driver_object->DriverStart;

			for (ULONG_PTR i = 0; i < device_ext_size; i++)
			{
				if (device_extension[i] == (ULONG_PTR)class_device_object && device_extension[i + 1] > (ULONG_PTR)class_driver_object)
				{
					mouse_obj->service_callback = (MouseClassServiceCallback)(device_extension[i + 1]);
					break;
				}
			}
			class_device_object = class_device_object->NextDevice;
		}
		hid_device_object = hid_device_object->AttachedDevice;
	}

	if (!mouse_obj->mouse_device)
	{
		PDEVICE_OBJECT target_device_object = class_driver_object->DeviceObject;
		while (target_device_object)
		{
			if (!target_device_object->NextDevice)
			{
				mouse_obj->mouse_device = target_device_object;
				break;
			}
			target_device_object = target_device_object->NextDevice;
		}
	}

	ObfDereferenceObject(class_driver_object);
	ObfDereferenceObject(hid_driver_object);

	return STATUS_SUCCESS;
}

NTSTATUS init_mouse_service()
{			
	if (!mouse_obj.service_callback || !mouse_obj.mouse_device)
		init_mouse(&mouse_obj);

	mouhid_base = reinterpret_cast<uint64_t>(get_module_base("mouhid.sys", &mouhid_size));

	return (mouse_obj.service_callback != nullptr && mouhid_base != 0 && mouhid_size != 0) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

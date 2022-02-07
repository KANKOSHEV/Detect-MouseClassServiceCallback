#pragma once

VOID(__fastcall* o_MouseClassServiceCallback)(PDEVICE_OBJECT, PMOUSE_INPUT_DATA, PMOUSE_INPUT_DATA, PULONG);

VOID __fastcall h_MouseClassServiceCallback(PDEVICE_OBJECT DeviceObject, PMOUSE_INPUT_DATA InputDataStart, PMOUSE_INPUT_DATA InputDataEnd, PULONG InputDataConsumed)
{
	auto ret_addr = reinterpret_cast<uint64_t>(_ReturnAddress());

	if (ret_addr < mouhid_base ||
		ret_addr > mouhid_base + mouhid_size)
	{
		DbgPrintEx(0, 0, "MouseClassServiceCallback call detect -> 0x%llX\n", ret_addr);
	}

	return o_MouseClassServiceCallback(DeviceObject, InputDataStart, InputDataEnd, InputDataConsumed);
}

NTSTATUS init_hook()
{
	void** p_func = reinterpret_cast<void**>(mouse_obj.service_callback);

	bool b_result = inline_hook((void*)h_MouseClassServiceCallback, p_func, (void**)&o_MouseClassServiceCallback);

	return b_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}
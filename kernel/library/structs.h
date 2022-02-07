#pragma once

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	PVOID SectionPointer;
	ULONG CheckSum;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _KLDR_DATA_TABLE_ENTRY
{
	/* 0x0000 */ struct _LIST_ENTRY InLoadOrderLinks;
	/* 0x0010 */ void* ExceptionTable;
	/* 0x0018 */ unsigned long ExceptionTableSize;
	/* 0x001c */ long Padding_687;
	/* 0x0020 */ void* GpValue;
	/* 0x0028 */ struct _NON_PAGED_DEBUG_INFO* NonPagedDebugInfo;
	/* 0x0030 */ void* DllBase;
	/* 0x0038 */ void* EntryPoint;
	/* 0x0040 */ unsigned long SizeOfImage;
	/* 0x0044 */ long Padding_688;
	/* 0x0048 */ struct _UNICODE_STRING FullDllName;
	/* 0x0058 */ struct _UNICODE_STRING BaseDllName;
	/* 0x0068 */ unsigned long Flags;
	/* 0x006c */ unsigned short LoadCount;
	union
	{
		union
		{
			struct /* bitfield */
			{
				/* 0x006e */ unsigned short SignatureLevel : 4; /* bit position: 0 */
				/* 0x006e */ unsigned short SignatureType : 3; /* bit position: 4 */
				/* 0x006e */ unsigned short Unused : 9; /* bit position: 7 */
			}; /* bitfield */
			/* 0x006e */ unsigned short EntireField;
		}; /* size: 0x0002 */
	} /* size: 0x0002 */ u1;
	/* 0x0070 */ void* SectionPointer;
	/* 0x0078 */ unsigned long CheckSum;
	/* 0x007c */ unsigned long CoverageSectionSize;
	/* 0x0080 */ void* CoverageSection;
	/* 0x0088 */ void* LoadedImports;
	/* 0x0090 */ void* Spare;
	/* 0x0098 */ unsigned long SizeOfImageNotRounded;
	/* 0x009c */ unsigned long TimeDateStamp;
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY; /* size: 0x00a0 */

typedef VOID(*MouseClassServiceCallback)(PDEVICE_OBJECT DeviceObject, PMOUSE_INPUT_DATA InputDataStart, PMOUSE_INPUT_DATA InputDataEnd, PULONG InputDataConsumed);

typedef struct _MOUSE_OBJECT
{
	PDEVICE_OBJECT mouse_device;
	MouseClassServiceCallback service_callback;
} MOUSE_OBJECT, *PMOUSE_OBJECT;

ULONG mouhid_size = 0;
uint64_t mouhid_base = 0;

MOUSE_OBJECT mouse_obj = { 0 };

extern "C"
{
	PLIST_ENTRY NTKERNELAPI PsLoadedModuleList;
	NTKERNELAPI PVOID NTAPI RtlFindExportedRoutineByName(_In_ PVOID ImageBase, _In_ PCCH RoutineName);
	NTKERNELAPI NTSTATUS ObReferenceObjectByName(IN PUNICODE_STRING ObjectName, IN ULONG Attributes, IN PACCESS_STATE PassedAccessState, IN ACCESS_MASK DesiredAccess, IN POBJECT_TYPE ObjectType, IN KPROCESSOR_MODE AccessMode, IN OUT PVOID ParseContext, OUT PVOID* Object);
}

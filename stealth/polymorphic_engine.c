#include "decoy_patterns.h"
#include "crypto_utils.h"

static VOID TransformCodePattern(PVOID Address, SIZE_T Size) {
    PUCHAR buffer = (PUCHAR)Address;
    ULONG seed = KeQueryTimeIncrement();

    // Modify instruction patterns while maintaining validity
    for (SIZE_T i = 0; i < Size; i++) {
        if (IsInstructionBoundary(buffer + i)) {
            ModifyInstruction(buffer + i, &i);
        }
    }
}

static VOID TransformDataStructure(PVOID Address, SIZE_T Size) {
    PULONG_PTR data = (PULONG_PTR)Address;
    SIZE_T count = Size / sizeof(ULONG_PTR);

    // Shuffle data while maintaining pointer relationships
    for (SIZE_T i = 0; i < count; i++) {
        if (IsPointerValue(data[i])) {
            data[i] = TransformPointer(data[i]);
        }
    }
}

NTSTATUS PolymorphicTransform(PDECOY_REGION Decoy) {
    KIRQL oldIrql;
    KeAcquireSpinLock(&Decoy->TransformLock, &oldIrql);

    // Create temporary buffer for transformation
    PVOID tempBuffer = ExAllocatePoolWithTag(NonPagedPool, Decoy->Size, 'myoP');
    if (!tempBuffer) {
        KeReleaseSpinLock(&Decoy->TransformLock, oldIrql);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // Perform transformation based on pattern type
    RtlCopyMemory(tempBuffer, Decoy->BaseAddress, Decoy->Size);
    
    switch (Decoy->Pattern.Type) {
        case PATTERN_CODE:
            TransformCodePattern(tempBuffer, Decoy->Size);
            break;
        case PATTERN_DATA:
            TransformDataStructure(tempBuffer, Decoy->Size);
            break;
    }

    // Apply transformation
    RtlCopyMemory(Decoy->BaseAddress, tempBuffer, Decoy->Size);
    ExFreePoolWithTag(tempBuffer, 'myoP');

    KeReleaseSpinLock(&Decoy->TransformLock, oldIrql);
    return STATUS_SUCCESS;
} 
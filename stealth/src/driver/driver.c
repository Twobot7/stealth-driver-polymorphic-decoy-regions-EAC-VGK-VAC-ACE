#include "../loader/loader.h"

LOADER_CONTEXT g_LoaderContext = NULL;

NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
) {
    NTSTATUS status;
    LOADER_CONFIG config = {
        .DecoyConfig = {
            .MinDecoySize = 1024,
            .MaxDecoySize = 4096,
            .DecoyCount = 10,
            .EnablePolymorphic = TRUE,
            .EnableBehavioralEmulation = TRUE,
            .UpdateInterval = 5000
        },
        .AutoStart = TRUE,
        .LoaderFlags = 0,
        .InitializationTimeout = 30000
    };

    status = InitializeLoader(&config, &g_LoaderContext);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (config.AutoStart) {
        status = StartLoader(g_LoaderContext);
        if (!NT_SUCCESS(status)) {
            CleanupLoader(g_LoaderContext);
            return status;
        }
    }

    // Set up driver unload routine
    DriverObject->DriverUnload = DriverUnload;

    return STATUS_SUCCESS;
}

VOID DriverUnload(
    IN PDRIVER_OBJECT DriverObject
) {
    if (g_LoaderContext) {
        StopLoader(g_LoaderContext);
        CleanupLoader(g_LoaderContext);
        g_LoaderContext = NULL;
    }
} 
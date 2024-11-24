#include "stealth_engine.h"
#include "crypto/entropy.h"
#include "protection/integrity.h"
#include "deception/decoy_manager.h"

static NTSTATUS InitializeProtectionSystems(PSTEALTH_ENGINE_CONTEXT Context) {
    NTSTATUS status;
    
    // Initialize polymorphic engine with advanced entropy
    Context->PolymorphicEngine = CreatePolymorphicEngine(
        Context->Config.PolymorphicInterval,
        Context->Config.EnableAdvancedHeuristics
    );
    
    if (!Context->PolymorphicEngine) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    // Initialize threat detection with behavioral analysis
    Context->ThreatDetector = CreateThreatDetector(
        Context->Config.ThreatSensitivity,
        Context->PolymorphicEngine
    );
    
    if (!Context->ThreatDetector) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    // Initialize integrity monitoring
    Context->IntegrityMonitor = CreateIntegrityMonitor(
        Context->Config.EnableAdvancedHeuristics
    );
    
    return STATUS_SUCCESS;
}

NTSTATUS InitializeStealthEngine(
    IN PDRIVER_OBJECT DriverObject,
    IN PSTEALTH_ENGINE_CONFIG Config,
    OUT PSTEALTH_ENGINE_CONTEXT* Context
) {
    *Context = ExAllocatePoolWithTag(NonPagedPool, 
                                   sizeof(STEALTH_ENGINE_CONTEXT),
                                   'htlS');
                                   
    if (!*Context) return STATUS_INSUFFICIENT_RESOURCES;
    
    RtlZeroMemory(*Context, sizeof(STEALTH_ENGINE_CONTEXT));
    KeInitializeSpinLock(&(*Context)->EngineLock);
    
    // Copy configuration
    RtlCopyMemory(&(*Context)->Config, Config, sizeof(STEALTH_ENGINE_CONFIG));
    
    // Initialize protection systems
    NTSTATUS status = InitializeProtectionSystems(*Context);
    if (!NT_SUCCESS(status)) {
        ShutdownStealthEngine(*Context, TRUE);
        return status;
    }
    
    // Initialize decoy system if enabled
    if (Config->Flags & STEALTH_ENGINE_DECEPTIVE) {
        status = InitializeDecoySystem(*Context);
        if (!NT_SUCCESS(status)) {
            ShutdownStealthEngine(*Context, TRUE);
            return status;
        }
    }
    
    (*Context)->IsInitialized = TRUE;
    return STATUS_SUCCESS;
} 
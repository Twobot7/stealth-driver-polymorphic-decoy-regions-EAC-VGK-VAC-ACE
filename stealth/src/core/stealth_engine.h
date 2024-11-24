#pragma once
#include <ntifs.h>
#include "common/types.h"
#include "protection/memory_guard.h"
#include "crypto/polymorphic.h"

// Advanced configuration flags
#define STEALTH_ENGINE_PARANOID      0x00000001
#define STEALTH_ENGINE_POLYMORPHIC   0x00000002
#define STEALTH_ENGINE_DECEPTIVE     0x00000004
#define STEALTH_ENGINE_AGGRESSIVE    0x00000008

typedef struct _STEALTH_ENGINE_CONFIG {
    ULONG Flags;
    ULONG PolymorphicInterval;
    ULONG DecoyCount;
    ULONG ThreatSensitivity;
    PVOID EncryptionContext;
    BOOLEAN EnableAdvancedHeuristics;
} STEALTH_ENGINE_CONFIG, *PSTEALTH_ENGINE_CONFIG;

typedef struct _STEALTH_ENGINE_CONTEXT {
    STEALTH_ENGINE_CONFIG Config;
    KSPIN_LOCK EngineLock;
    LIST_ENTRY ProtectedRegions;
    PVOID ThreatDetector;
    PVOID PolymorphicEngine;
    PVOID DecoyManager;
    PVOID IntegrityMonitor;
    BOOLEAN IsInitialized;
} STEALTH_ENGINE_CONTEXT, *PSTEALTH_ENGINE_CONTEXT;

NTSTATUS InitializeStealthEngine(
    IN PDRIVER_OBJECT DriverObject,
    IN PSTEALTH_ENGINE_CONFIG Config,
    OUT PSTEALTH_ENGINE_CONTEXT* Context
);

VOID ShutdownStealthEngine(
    IN PSTEALTH_ENGINE_CONTEXT Context,
    IN BOOLEAN SecureCleanup
); 
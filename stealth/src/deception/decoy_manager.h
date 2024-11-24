#pragma once
#include "../core/stealth_engine.h"

typedef struct _DECOY_MANAGER_CONFIG {
    ULONG MinDecoySize;
    ULONG MaxDecoySize;
    ULONG DecoyCount;
    BOOLEAN EnablePolymorphic;
    BOOLEAN EnableBehavioralEmulation;
    ULONG UpdateInterval;
} DECOY_MANAGER_CONFIG, *PDECOY_MANAGER_CONFIG;

NTSTATUS CreateDecoyManager(
    IN PSTEALTH_ENGINE_CONTEXT EngineContext,
    IN PDECOY_MANAGER_CONFIG Config,
    OUT PVOID* DecoyManager
);

VOID UpdateDecoys(
    IN PVOID DecoyManager,
    IN BOOLEAN ForceUpdate
);

VOID CleanupDecoyManager(
    IN PVOID DecoyManager
); 
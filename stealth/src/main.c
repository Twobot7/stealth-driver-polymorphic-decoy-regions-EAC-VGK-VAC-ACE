#include <windows.h>
#include <stdio.h>
#include "loader/loader.h"

BOOL SetupConsole(void) {
    if (!AllocConsole()) {
        return FALSE;
    }
    
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    return TRUE;
}

int main(int argc, char* argv[]) {
    LOADER_CONTEXT loaderContext = NULL;
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

    SetupConsole();
    printf("Initializing loader...\n");

    if (!InitializeLoader(&config, &loaderContext)) {
        printf("Failed to initialize loader\n");
        return 1;
    }

    if (config.AutoStart) {
        if (!StartLoader(loaderContext)) {
            printf("Failed to start loader\n");
            CleanupLoader(loaderContext);
            return 1;
        }
    }

    printf("Loader running. Press Enter to exit...\n");
    getchar();

    StopLoader(loaderContext);
    CleanupLoader(loaderContext);
    
    return 0;
} 
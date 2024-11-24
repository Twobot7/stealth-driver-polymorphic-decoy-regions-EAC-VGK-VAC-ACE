#pragma once
#include <windows.h>
#include <stdio.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"

typedef struct _DECOY_MANAGER_CONFIG {
    ULONG MinDecoySize;
    ULONG MaxDecoySize;
    ULONG DecoyCount;
    BOOLEAN EnablePolymorphic;
    BOOLEAN EnableBehavioralEmulation;
    ULONG UpdateInterval;
} DECOY_MANAGER_CONFIG, *PDECOY_MANAGER_CONFIG;

typedef struct _LOADER_CONFIG {
    DECOY_MANAGER_CONFIG DecoyConfig;
    BOOLEAN AutoStart;
    ULONG LoaderFlags;
    ULONG InitializationTimeout;
} LOADER_CONFIG, *PLOADER_CONFIG;

typedef struct _LOADER_CRYPTO {
    BYTE AESKey[32];  // 256-bit key
    BYTE IV[16];      // 128-bit IV
    BOOL IsInitialized;
} LOADER_CRYPTO, *PLOADER_CRYPTO;

typedef struct _LOADER_CONTEXT {
    LOADER_CONFIG Config;
    HANDLE EngineHandle;
    PDECOY_MANAGER DecoyManager;
    BOOL IsInitialized;
    HWND WindowHandle;
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
    IDXGISwapChain* SwapChain;
    ID3D11RenderTargetView* RenderTargetView;
    LOADER_CRYPTO Crypto;
    PBYTE EncryptedBytes;
    SIZE_T BytesLength;
} LOADER_CONTEXT, *PLOADER_CONTEXT;

BOOL InitializeLoader(
    IN PLOADER_CONFIG Config,
    OUT PLOADER_CONTEXT* LoaderContext
);

BOOL StartLoader(
    IN PLOADER_CONTEXT LoaderContext
);

BOOL StopLoader(
    IN PLOADER_CONTEXT LoaderContext
);

VOID CleanupLoader(
    IN PLOADER_CONTEXT LoaderContext
);

BOOL RenderLoaderUI(IN PLOADER_CONTEXT LoaderContext);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); 
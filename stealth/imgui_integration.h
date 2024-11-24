#pragma once
#include "stealth_types.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>

// Forward declarations
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

typedef struct _IMGUI_CONTEXT {
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
    IDXGISwapChain* SwapChain;
    ID3D11RenderTargetView* RenderTargetView;
    HWND WindowHandle;
    bool IsInitialized;
} IMGUI_CONTEXT, *PIMGUI_CONTEXT;

NTSTATUS InitializeImGui(PSTEALTH_CONTEXT Context);
void RenderImGuiDebugWindow(PSTEALTH_CONTEXT Context);
void CleanupImGui(PSTEALTH_CONTEXT Context); 
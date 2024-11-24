#include "loader.h"
#include "../deception/decoy_manager.h"
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <wincrypt.h>
#include <bcrypt.h>

BOOL InitializeLoader(
    IN PLOADER_CONFIG Config,
    OUT PLOADER_CONTEXT* LoaderContext
) {
    PLOADER_CONTEXT context;

    context = (PLOADER_CONTEXT)malloc(sizeof(LOADER_CONTEXT));
    if (context == NULL) {
        printf("Failed to allocate loader context\n");
        return FALSE;
    }

    ZeroMemory(context, sizeof(LOADER_CONTEXT));
    memcpy(&context->Config, Config, sizeof(LOADER_CONFIG));

    // Initialize engine
    context->EngineHandle = CreateMutexA(NULL, FALSE, "StealthEngine");
    if (context->EngineHandle == NULL) {
        printf("Failed to initialize engine\n");
        free(context);
        return FALSE;
    }

    // Initialize decoy manager
    if (!CreateDecoyManager(context->EngineHandle, &Config->DecoyConfig, &context->DecoyManager)) {
        printf("Failed to create decoy manager\n");
        CleanupLoader(context);
        return FALSE;
    }

    // Create Window
    WNDCLASSEX wc = { 
        sizeof(WNDCLASSEX), 
        CS_CLASSDC, 
        WndProc, 
        0, 0, 
        GetModuleHandle(NULL), 
        NULL, NULL, NULL, NULL, 
        L"Loader Window", 
        NULL 
    };
    RegisterClassEx(&wc);
    context->WindowHandle = CreateWindow(
        wc.lpszClassName,
        L"Loader",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        NULL, NULL,
        wc.hInstance,
        NULL
    );

    // Initialize Direct3D
    DXGI_SWAP_CHAIN_DESC sd = {
        .BufferDesc.Width = 0,
        .BufferDesc.Height = 0,
        .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .BufferDesc.RefreshRate.Numerator = 60,
        .BufferDesc.RefreshRate.Denominator = 1,
        .SampleDesc.Count = 1,
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .OutputWindow = context->WindowHandle,
        .Windowed = TRUE,
        .SwapEffect = DXGI_SWAP_EFFECT_DISCARD
    };

    D3D_FEATURE_LEVEL featureLevel;
    if (D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
        NULL, 0, D3D11_SDK_VERSION,
        &sd, &context->SwapChain,
        &context->Device, &featureLevel,
        &context->DeviceContext) != S_OK) {
        printf("Failed to create D3D11 device\n");
        CleanupLoader(context);
        return FALSE;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(context->WindowHandle);
    ImGui_ImplDX11_Init(context->Device, context->DeviceContext);

    // Initialize crypto structure
    ZeroMemory(&context->Crypto, sizeof(LOADER_CRYPTO));
    context->EncryptedBytes = NULL;
    context->BytesLength = 0;

    *LoaderContext = context;
    return TRUE;
}

BOOL StartLoader(
    IN PLOADER_CONTEXT LoaderContext
) {
    if (LoaderContext->IsInitialized) {
        printf("Loader already initialized\n");
        return FALSE;
    }

    UpdateDecoys(LoaderContext->DecoyManager, TRUE);
    LoaderContext->IsInitialized = TRUE;
    printf("Loader started successfully\n");

    return TRUE;
}

BOOL StopLoader(
    IN PLOADER_CONTEXT LoaderContext
) {
    if (!LoaderContext->IsInitialized) {
        printf("Loader not initialized\n");
        return FALSE;
    }

    LoaderContext->IsInitialized = FALSE;
    printf("Loader stopped successfully\n");
    return TRUE;
}

BOOL RenderLoaderUI(IN PLOADER_CONTEXT LoaderContext) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Set window size and position
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

    // Begin main window with better styling
    ImGui::Begin("Loader Control Panel", nullptr, 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoResize);

    // Add spacing and header
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Loader Status");
    ImGui::Separator();

    // Status section
    ImGui::BeginGroup();
    ImGui::Text("Current Status: ");
    ImGui::SameLine();
    if (LoaderContext->IsInitialized) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Running");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stopped");
    }
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Control buttons
    if (!LoaderContext->IsInitialized) {
        if (ImGui::Button("Start Loader", ImVec2(120, 30))) {
            StartLoader(LoaderContext);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Initialize and start the loader");
        }
    } else {
        if (ImGui::Button("Stop Loader", ImVec2(120, 30))) {
            StopLoader(LoaderContext);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Stop and cleanup the loader");
        }
    }

    // Configuration section
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Configuration");
    
    // Decoy settings
    if (ImGui::TreeNode("Decoy Settings")) {
        ImGui::DragInt("Min Decoy Size", (int*)&LoaderContext->Config.DecoyConfig.MinDecoySize, 1.0f, 1, 1000);
        ImGui::DragInt("Max Decoy Size", (int*)&LoaderContext->Config.DecoyConfig.MaxDecoySize, 1.0f, 1, 1000);
        ImGui::DragInt("Decoy Count", (int*)&LoaderContext->Config.DecoyConfig.DecoyCount, 1.0f, 1, 100);
        ImGui::Checkbox("Enable Polymorphic", (bool*)&LoaderContext->Config.DecoyConfig.EnablePolymorphic);
        ImGui::TreePop();
    }

    // Add crypto status section
    if (ImGui::TreeNode("Crypto Status")) {
        ImGui::Text("Crypto Initialized: %s", 
            LoaderContext->Crypto.IsInitialized ? "Yes" : "No");
        
        if (LoaderContext->EncryptedBytes) {
            ImGui::Text("Encrypted Bytes: %zu bytes", LoaderContext->BytesLength);
        } else {
            ImGui::Text("No encrypted data loaded");
        }
        
        ImGui::TreePop();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    LoaderContext->SwapChain->Present(1, 0);

    return TRUE;
}

VOID CleanupLoader(
    IN PLOADER_CONTEXT LoaderContext
) {
    // Add ImGui cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (LoaderContext->RenderTargetView) {
        LoaderContext->RenderTargetView->Release();
    }
    if (LoaderContext->SwapChain) {
        LoaderContext->SwapChain->Release();
    }
    if (LoaderContext->DeviceContext) {
        LoaderContext->DeviceContext->Release();
    }
    if (LoaderContext->Device) {
        LoaderContext->Device->Release();
    }
    if (LoaderContext->WindowHandle) {
        DestroyWindow(LoaderContext->WindowHandle);
    }

    if (LoaderContext->DecoyManager) {
        CleanupDecoyManager(LoaderContext->DecoyManager);
    }

    if (LoaderContext->EngineHandle) {
        CloseHandle(LoaderContext->EngineHandle);
    }

    if (LoaderContext->EncryptedBytes) {
        SecureZeroMemory(LoaderContext->EncryptedBytes, LoaderContext->BytesLength);
        free(LoaderContext->EncryptedBytes);
    }
    
    SecureZeroMemory(&LoaderContext->Crypto, sizeof(LOADER_CRYPTO));

    free(LoaderContext);
}

// Add window procedure
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_SIZE:
            // Handle window resizing
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL InitializeCrypto(IN PLOADER_CONTEXT LoaderContext, IN PBYTE AESKey) {
    if (LoaderContext->Crypto.IsInitialized) {
        return FALSE;
    }

    // Copy the AES key
    memcpy(LoaderContext->Crypto.AESKey, AESKey, 32);
    
    // Generate random IV
    BCRYPT_ALG_HANDLE hAlg;
    if (FAILED(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_RNG_ALGORITHM, NULL, 0))) {
        return FALSE;
    }
    
    if (FAILED(BCryptGenRandom(hAlg, LoaderContext->Crypto.IV, 16, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return FALSE;
    }
    
    BCryptCloseAlgorithmProvider(hAlg, 0);
    LoaderContext->Crypto.IsInitialized = TRUE;
    return TRUE;
}

BOOL ProcessEncryptedBytes(
    IN PLOADER_CONTEXT LoaderContext,
    IN PBYTE EncryptedData,
    IN SIZE_T DataLength
) {
    if (!LoaderContext->Crypto.IsInitialized) {
        return FALSE;
    }

    // Store encrypted bytes
    LoaderContext->EncryptedBytes = (PBYTE)malloc(DataLength);
    if (LoaderContext->EncryptedBytes == NULL) {
        return FALSE;
    }
    
    memcpy(LoaderContext->EncryptedBytes, EncryptedData, DataLength);
    LoaderContext->BytesLength = DataLength;
    
    return TRUE;
} 
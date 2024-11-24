#include "imgui_integration.h"
#include "decoy_protection.h"
#include "threat_detection.h"

static IMGUI_CONTEXT g_ImGuiContext = { 0 };

static void RenderDecoyInfo(PDECOY_REGION Decoy) {
    if (ImGui::TreeNode("Decoy Region")) {
        ImGui::Text("Base Address: %p", Decoy->BaseAddress);
        ImGui::Text("Size: %zu bytes", Decoy->Size);
        ImGui::Text("Access Count: %lu", Decoy->AccessCount);
        ImGui::Text("Status: %s", Decoy->IsActive ? "Active" : "Inactive");
        
        if (ImGui::Button("Regenerate Content")) {
            GenerateDecoyContent(Decoy->BaseAddress, Decoy->Size);
        }
        
        ImGui::TreePop();
    }
}

static void RenderThreatInfo(PTHREAT_DETECTION_STATE ThreatState) {
    if (ImGui::TreeNode("Threat Detection")) {
        ImGui::Text("Current Threat Level: %d", ThreatState->CurrentThreatLevel);
        
        if (ImGui::BeginTable("Threat History", 3)) {
            ImGui::TableSetupColumn("Time");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Level");
            ImGui::TableHeadersRow();
            
            for (ULONG i = 0; i < MAX_THREAT_HISTORY; i++) {
                PTHREAT_EVENT event = &ThreatState->History[i];
                if (event->Timestamp.QuadPart == 0) continue;
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%lld", event->Timestamp.QuadPart);
                ImGui::TableNextColumn();
                ImGui::Text("%d", event->Type);
                ImGui::TableNextColumn();
                ImGui::Text("%d", event->ThreatLevel);
            }
            ImGui::EndTable();
        }
        
        ImGui::TreePop();
    }
}

NTSTATUS InitializeImGui(PSTEALTH_CONTEXT Context) {
    // Create window and D3D11 device (implementation needed)
    // ...

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_ImGuiContext.WindowHandle);
    ImGui_ImplDX11_Init(g_ImGuiContext.Device, g_ImGuiContext.DeviceContext);

    g_ImGuiContext.IsInitialized = true;
    return STATUS_SUCCESS;
}

void RenderImGuiDebugWindow(PSTEALTH_CONTEXT Context) {
    if (!g_ImGuiContext.IsInitialized) return;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Stealth System Debug");
    
    if (ImGui::CollapsingHeader("Decoy Regions")) {
        PLIST_ENTRY entry = Context->DecoyList.Flink;
        while (entry != &Context->DecoyList) {
            PDECOY_REGION decoy = CONTAINING_RECORD(entry, DECOY_REGION, ListEntry);
            RenderDecoyInfo(decoy);
            entry = entry->Flink;
        }
    }

    if (ImGui::CollapsingHeader("Threat Detection")) {
        RenderThreatInfo(Context->ThreatState);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CleanupImGui(PSTEALTH_CONTEXT Context) {
    if (!g_ImGuiContext.IsInitialized) return;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (g_ImGuiContext.RenderTargetView) g_ImGuiContext.RenderTargetView->Release();
    if (g_ImGuiContext.SwapChain) g_ImGuiContext.SwapChain->Release();
    if (g_ImGuiContext.DeviceContext) g_ImGuiContext.DeviceContext->Release();
    if (g_ImGuiContext.Device) g_ImGuiContext.Device->Release();

    g_ImGuiContext.IsInitialized = false;
} 
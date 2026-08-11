#include "imgui/imgui.h"

#include <math.h>
#include <stdio.h>

int main(void)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(1280.0f, 720.0f);
    io.DeltaTime = 1.0f / 60.0f;

    unsigned char *pixels = nullptr;
    int tex_w = 0;
    int tex_h = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &tex_w, &tex_h);
    printf("imgui %s, font atlas %dx%d\n", IMGUI_VERSION, tex_w, tex_h);

    ImVec2 button_center = ImVec2(-1.0f, -1.0f);
    int clicks = 0;

    for (int frame = 0; frame < 12; frame++)
    {
        if (button_center.x >= 0.0f)
        {
            io.AddMousePosEvent(button_center.x, button_center.y);
            io.AddMouseButtonEvent(0, (frame % 3) == 1);
        }

        float wave = 0.5f + 0.5f * sinf((float)frame * 0.4f);
        bool even = (frame % 2) == 0;

        ImGui::NewFrame();
        ImGui::Begin("spn headless imgui");
        ImGui::Text("frame %d of 12", frame);
        if (ImGui::Button("click me"))
            clicks++;
        ImVec2 rect_min = ImGui::GetItemRectMin();
        ImVec2 rect_max = ImGui::GetItemRectMax();
        button_center = ImVec2((rect_min.x + rect_max.x) * 0.5f, (rect_min.y + rect_max.y) * 0.5f);
        ImGui::SliderFloat("wave", &wave, 0.0f, 1.0f);
        ImGui::Checkbox("even frame", &even);
        if (ImGui::TreeNodeEx("details", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("counters", 2, ImGuiTableFlags_Borders))
            {
                for (int row = 0; row < 3; row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("harmonic %d", row + 1);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.4f", wave * (float)(row + 1));
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        ImGui::End();
        ImGui::Render();

        const ImDrawData *draw = ImGui::GetDrawData();
        printf("frame %2d: display %.0fx%.0f draw_lists %d vtx %5d idx %5d wave %.4f clicks %d\n",
               frame,
               draw->DisplaySize.x,
               draw->DisplaySize.y,
               draw->CmdListsCount,
               draw->TotalVtxCount,
               draw->TotalIdxCount,
               wave,
               clicks);
    }

    ImGui::DestroyContext();
    printf("done, %d clicks delivered with no display\n", clicks);
    return 0;
}

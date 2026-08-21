#include "dcimgui.h"

#include <math.h>
#include <stdio.h>

int main(void)
{
    ImGui_CreateContext(NULL);

    ImGuiIO *io = ImGui_GetIO();
    io->IniFilename = NULL;
    io->DisplaySize = (ImVec2){1280.0f, 720.0f};
    io->DeltaTime = 1.0f / 60.0f;

    unsigned char *pixels = NULL;
    int tex_w = 0;
    int tex_h = 0;
    ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &pixels, &tex_w, &tex_h, NULL);
    printf("imgui %s, font atlas %dx%d\n", ImGui_GetVersion(), tex_w, tex_h);

    ImVec2 button_center = (ImVec2){-1.0f, -1.0f};
    int clicks = 0;

    for (int frame = 0; frame < 12; frame++)
    {
        if (button_center.x >= 0.0f)
        {
            ImGuiIO_AddMousePosEvent(io, button_center.x, button_center.y);
            ImGuiIO_AddMouseButtonEvent(io, 0, (frame % 3) == 1);
        }

        float wave = 0.5f + 0.5f * sinf((float)frame * 0.4f);
        bool even = (frame % 2) == 0;

        ImGui_NewFrame();
        ImGui_Begin("spn headless dcimgui", NULL, 0);
        ImGui_Text("frame %d of 12", frame);
        if (ImGui_Button("click me"))
            clicks++;
        ImVec2 rect_min = ImGui_GetItemRectMin();
        ImVec2 rect_max = ImGui_GetItemRectMax();
        button_center = (ImVec2){(rect_min.x + rect_max.x) * 0.5f, (rect_min.y + rect_max.y) * 0.5f};
        ImGui_SliderFloat("wave", &wave, 0.0f, 1.0f);
        ImGui_Checkbox("even frame", &even);
        if (ImGui_TreeNodeEx("details", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui_BeginTable("counters", 2, ImGuiTableFlags_Borders))
            {
                for (int row = 0; row < 3; row++)
                {
                    ImGui_TableNextRow();
                    ImGui_TableSetColumnIndex(0);
                    ImGui_Text("harmonic %d", row + 1);
                    ImGui_TableSetColumnIndex(1);
                    ImGui_Text("%.4f", wave * (float)(row + 1));
                }
                ImGui_EndTable();
            }
            ImGui_TreePop();
        }
        ImGui_End();
        ImGui_Render();

        const ImDrawData *draw = ImGui_GetDrawData();
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

    ImGui_DestroyContext(NULL);
    printf("done, %d clicks delivered with no display\n", clicks);
    return 0;
}

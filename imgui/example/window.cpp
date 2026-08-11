#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl2.h"

#include "SDL.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SPN_GL_COLOR_BUFFER_BIT 0x00004000u
#define SPN_GL_VENDOR 0x1F00u
#define SPN_GL_RENDERER 0x1F01u
#define SPN_GL_VERSION 0x1F02u
#define SPN_GL_RGBA 0x1908u
#define SPN_GL_UNSIGNED_BYTE 0x1401u
#define SPN_GL_PACK_ALIGNMENT 0x0D05u

typedef void (*gl_viewport_fn)(int, int, int, int);
typedef void (*gl_clear_color_fn)(float, float, float, float);
typedef void (*gl_clear_fn)(unsigned int);
typedef void (*gl_pixel_storei_fn)(unsigned int, int);
typedef void (*gl_read_pixels_fn)(int, int, int, int, unsigned int, unsigned int, void *);
typedef const unsigned char *(*gl_get_string_fn)(unsigned int);

static gl_viewport_fn gl_viewport;
static gl_clear_color_fn gl_clear_color;
static gl_clear_fn gl_clear;
static gl_pixel_storei_fn gl_pixel_storei;
static gl_read_pixels_fn gl_read_pixels;
static gl_get_string_fn gl_get_string;

static const char *gl_string(unsigned int name)
{
    const unsigned char *s = gl_get_string ? gl_get_string(name) : nullptr;
    return s ? (const char *)s : "<unavailable>";
}

static void capture(SDL_Window *window, const char *path)
{
    int w = 0;
    int h = 0;
    SDL_GL_GetDrawableSize(window, &w, &h);

    size_t bytes = (size_t)w * (size_t)h * 4u;
    unsigned char *raw = (unsigned char *)SDL_malloc(bytes);
    unsigned char *flipped = (unsigned char *)SDL_malloc(bytes);
    if (!raw || !flipped)
    {
        printf("capture: out of memory\n");
        SDL_free(raw);
        SDL_free(flipped);
        return;
    }

    gl_pixel_storei(SPN_GL_PACK_ALIGNMENT, 1);
    gl_read_pixels(0, 0, w, h, SPN_GL_RGBA, SPN_GL_UNSIGNED_BYTE, raw);
    for (int y = 0; y < h; y++)
        SDL_memcpy(flipped + (size_t)y * (size_t)w * 4u, raw + (size_t)(h - 1 - y) * (size_t)w * 4u, (size_t)w * 4u);

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(flipped, w, h, 32, w * 4, 0x000000ffu, 0x0000ff00u, 0x00ff0000u, 0u);
    if (!surface || SDL_SaveBMP(surface, path) != 0)
        printf("capture: save failed: %s\n", SDL_GetError());
    else
        printf("capture: wrote %s (%dx%d)\n", path, w, h);

    unsigned int distinct = 0;
    unsigned int bright = 0;
    unsigned int first = 0xffffffffu;
    for (size_t px = 0; px < (size_t)w * (size_t)h; px++)
    {
        const unsigned char *p = flipped + px * 4u;
        unsigned int rgb = ((unsigned int)p[0] << 16) | ((unsigned int)p[1] << 8) | (unsigned int)p[2];
        if (first == 0xffffffffu)
            first = rgb;
        else if (rgb != first)
            distinct++;
        if (p[0] > 180 && p[1] > 180 && p[2] > 180)
            bright++;
    }
    printf("capture: corner #%06x, %u px differ from corner, %u bright px\n", first, distinct, bright);

    for (int probe = 1; probe <= 3; probe++)
    {
        int x = w * probe / 4;
        int y = h * probe / 4;
        const unsigned char *p = flipped + ((size_t)y * (size_t)w + (size_t)x) * 4u;
        printf("capture: pixel (%4d,%4d) = #%02x%02x%02x\n", x, y, p[0], p[1], p[2]);
    }

    if (surface)
        SDL_FreeSurface(surface);
    SDL_free(raw);
    SDL_free(flipped);
}

int main(int argc, char **argv)
{
    double seconds = argc > 1 ? atof(argv[1]) : 3.0;
    const char *bmp_path = argc > 2 ? argv[2] : "imgui-window.bmp";
    if (seconds <= 0.0)
        seconds = 3.0;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *window = SDL_CreateWindow("spn imgui sdl2 + opengl3",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1000,
                                          700,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl = SDL_GL_CreateContext(window);
    if (!gl)
    {
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_GL_MakeCurrent(window, gl);
    SDL_GL_SetSwapInterval(1);

    gl_viewport = (gl_viewport_fn)SDL_GL_GetProcAddress("glViewport");
    gl_clear_color = (gl_clear_color_fn)SDL_GL_GetProcAddress("glClearColor");
    gl_clear = (gl_clear_fn)SDL_GL_GetProcAddress("glClear");
    gl_pixel_storei = (gl_pixel_storei_fn)SDL_GL_GetProcAddress("glPixelStorei");
    gl_read_pixels = (gl_read_pixels_fn)SDL_GL_GetProcAddress("glReadPixels");
    gl_get_string = (gl_get_string_fn)SDL_GL_GetProcAddress("glGetString");
    if (!gl_viewport || !gl_clear_color || !gl_clear || !gl_pixel_storei || !gl_read_pixels || !gl_get_string)
    {
        printf("failed to resolve core GL entry points\n");
        return 1;
    }

    int win_w = 0;
    int win_h = 0;
    SDL_GetWindowSize(window, &win_w, &win_h);
    printf("video driver: %s\n", SDL_GetCurrentVideoDriver());
    printf("gl vendor:    %s\n", gl_string(SPN_GL_VENDOR));
    printf("gl renderer:  %s\n", gl_string(SPN_GL_RENDERER));
    printf("gl version:   %s\n", gl_string(SPN_GL_VERSION));
    printf("window size:  %dx%d\n", win_w, win_h);
    printf("imgui:        %s, running %.2fs\n", IMGUI_VERSION, seconds);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    ImVec4 clear_color = ImVec4(0.16f, 0.22f, 0.31f, 1.0f);
    bool show_demo = true;
    bool animate = true;
    float wave[96] = {};
    int clicks = 0;
    int frames = 0;
    int draw_lists = 0;
    int vtx = 0;
    int idx = 0;
    bool done = false;

    Uint64 start = SDL_GetTicks64();
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        double elapsed = (double)(SDL_GetTicks64() - start) / 1000.0;
        if (animate)
        {
            for (int i = 0; i < (int)IM_ARRAYSIZE(wave); i++)
                wave[i] = sinf((float)elapsed * 3.0f + (float)i * 0.18f) * cosf((float)i * 0.05f);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (show_demo)
            ImGui::ShowDemoWindow(&show_demo);

        ImGui::SetNextWindowPos(ImVec2(24.0f, 24.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420.0f, 460.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("spn imgui window");
        ImGui::Text("SDL2 + OpenGL3 backends, built by spn.");
        ImGui::Text("driver %s, gl %s", SDL_GetCurrentVideoDriver(), gl_string(SPN_GL_VERSION));
        ImGui::Separator();
        if (ImGui::Button("click me"))
            clicks++;
        ImGui::SameLine();
        ImGui::Text("clicks = %d", clicks);
        ImGui::SliderFloat3("clear color", (float *)&clear_color, 0.0f, 1.0f);
        ImGui::ColorEdit3("swatch", (float *)&clear_color);
        ImGui::Checkbox("animate", &animate);
        ImGui::SameLine();
        ImGui::Checkbox("demo window", &show_demo);
        ImGui::PlotLines("wave", wave, IM_ARRAYSIZE(wave), 0, nullptr, -1.2f, 1.2f, ImVec2(0.0f, 90.0f));
        ImGui::PlotHistogram("bars", wave, 24, 0, nullptr, -1.2f, 1.2f, ImVec2(0.0f, 60.0f));
        if (ImGui::TreeNodeEx("frame stats", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("stats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("metric");
                ImGui::TableSetupColumn("value");
                ImGui::TableHeadersRow();
                const char *names[] = { "frame", "elapsed", "framerate", "draw lists", "vertices" };
                for (int row = 0; row < IM_ARRAYSIZE(names); row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(names[row]);
                    ImGui::TableSetColumnIndex(1);
                    switch (row)
                    {
                        case 0: ImGui::Text("%d", frames); break;
                        case 1: ImGui::Text("%.2f s", elapsed); break;
                        case 2: ImGui::Text("%.1f fps", io.Framerate); break;
                        case 3: ImGui::Text("%d", draw_lists); break;
                        default: ImGui::Text("%d", vtx); break;
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        ImGui::ProgressBar((float)(elapsed / seconds), ImVec2(-1.0f, 0.0f), "run progress");
        ImGui::End();

        ImGui::Render();
        gl_viewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        gl_clear_color(clear_color.x, clear_color.y, clear_color.z, 1.0f);
        gl_clear(SPN_GL_COLOR_BUFFER_BIT);
        ImDrawData *draw = ImGui::GetDrawData();
        draw_lists = draw->CmdListsCount;
        vtx = draw->TotalVtxCount;
        idx = draw->TotalIdxCount;
        ImGui_ImplOpenGL3_RenderDrawData(draw);

        frames++;
        if (elapsed >= seconds)
            done = true;
        if (done)
            capture(window, bmp_path);

        SDL_GL_SwapWindow(window);
    }

    printf("frames: %d, framerate %.1f, draw lists %d, vtx %d, idx %d, clicks %d\n",
           frames,
           io.Framerate,
           draw_lists,
           vtx,
           idx,
           clicks);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

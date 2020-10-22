#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_sdl.h"
#include "Vendor/imgui/imgui_impl_opengl3.h"
#include "Vendor/imgui/metrics_gui/include/metrics_gui/metrics_gui.h"


#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>

#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "Vendor/stb_image/stb_image.h"



#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <SDL.h>

#include "Camera.h"
#include "Input.h"


#include "Renderer/Texture.h"
#include "Renderer/Renderer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Frustum.h"

#include "Terrain/GrassMesh.h"
#include "Terrain/TerrainMeshGenerator.h"
#include "Terrain/TerrainRenderable.h"

struct pair_hash 
{
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const 
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2;
    }
};

struct TerrainSettings
{
    int chunk_dimensions = 32;
    int max_terrain_height = 512;
    int generation_range = 16.f;
    int scale = 4.f;
};
TerrainSettings settings;


std::unique_ptr<Renderer> renderer;
std::unique_ptr<Input> input;

SDL_Window* window;
SDL_GLContext gl_context;

bool running = true;

void InitializeWindow(int windowW, int windowH);
bool DetermineTerrainSettings(const ImGuiIO& io);

int main(int argc, char** argv)
{
    InitializeWindow(1280, 720);
    const ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    renderer = std::make_unique<Renderer>();
    input = std::make_unique<Input>([]() { running = false; });

    if (!DetermineTerrainSettings(io))
    {
        return 0;
    }


    TerrainChunkGenerator gen(settings.chunk_dimensions, settings.max_terrain_height, settings.scale);
    std::unordered_map<Coords, std::unique_ptr<TerrainRenderable>, pair_hash> renderables;


    Camera cam;
    uint32_t prevTime = SDL_GetTicks();
    glm::vec2 prevMousePos = input->GetMousePos();

    float elapsed = 0;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    MetricsGuiMetric frameTimeMetric("Frame time", "s", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
    frameTimeMetric.mSelected = true;
    MetricsGuiPlot frameTimePlot;
    frameTimePlot.mShowAverage = true;
    frameTimePlot.mShowLegendAverage = true;
    frameTimePlot.mShowLegendColor = false;
    frameTimePlot.AddMetric(&frameTimeMetric);
    
    while (running)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        uint32_t currentTime = SDL_GetTicks();
        float dt = static_cast<float>(currentTime - prevTime) / 1000.f;
        prevTime = currentTime;
        elapsed += dt;

        frameTimeMetric.AddNewValue(dt);
        frameTimePlot.UpdateAxes();

        input->Update();

        static bool render_terrain = true;
        static bool render_skybox = true;

        ImGui::Begin("Debug");
        {
            ImGui::Checkbox("Render Terrain", &render_terrain);
            ImGui::Checkbox("Render Skybox", &render_skybox);
            ImGui::Checkbox("Render wireframes", &render_wireframe);

            ImGui::DragFloat("Terrain Scale", &terrain_scale, 1, 0.25, 32);

            ImGui::Spacing();

            if (ImGui::CollapsingHeader("FrameTime Info", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Separator();
                frameTimePlot.DrawHistory();
            }
        }
        ImGui::End();


        glm::vec2 mousePos = input->GetMousePos();
        glm::vec2 mouseDelta = mousePos - prevMousePos;
        prevMousePos = mousePos;

        if (input->IsButtonDown(SDL_BUTTON_LEFT))
        {
            cam.ProcessMouseMovement(mouseDelta.x, mouseDelta.y);
        }

        float speed = 64;
        if (input->IsKeyDown(SDLK_a)) cam.ProcessKeyboard(Camera_Movement::LEFT, dt);
        if (input->IsKeyDown(SDLK_d)) cam.ProcessKeyboard(Camera_Movement::RIGHT, dt);

        if (input->IsKeyDown(SDLK_w)) cam.ProcessKeyboard(Camera_Movement::FORWARD, dt);
        if (input->IsKeyDown(SDLK_s)) cam.ProcessKeyboard(Camera_Movement::BACKWARD, dt);

        if (input->IsKeyDown(SDLK_LSHIFT)) cam.ProcessKeyboard(Camera_Movement::DOWN, dt);
        if (input->IsKeyDown(SDLK_SPACE)) cam.ProcessKeyboard(Camera_Movement::UP, dt);


        int chunkX = static_cast<int>(cam.Position.x / settings.chunk_dimensions / settings.scale);
        int chunkZ = static_cast<int>(cam.Position.z / settings.chunk_dimensions / settings.scale);
        int loadRange = settings.generation_range;

        for (int x = -loadRange; x < loadRange; x++)
        {
            for (int z = -loadRange; z < loadRange; z++)
            {
                if (!gen.HasGenerated(chunkX + x, chunkZ + z))
                {
                    std::unique_ptr<TerrainMesh> chunk = gen.GenerateChunk(chunkX + x, chunkZ + z);
                    renderables[Coords(chunkX + x, chunkZ + z)] = (std::make_unique<TerrainRenderable>(*chunk, *renderer));
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glm::mat4 skyboxView = glm::mat4(glm::mat3(cam.GetViewMatrix()));
        glm::mat4 proj = glm::perspective(glm::pi<float>() / 4.0f, (float)w / (float)h, 0.1f, 5000.f);

        Frustum frustum(proj * cam.GetViewMatrix());

        
        if (render_skybox)
        {
            renderer->RenderSkybox(skyboxView, proj);
        }

        renderer->RenderWireframes(render_wireframe);

        if (render_terrain)
        {
            float offset0 = glm::sin(elapsed) * glm::sin(elapsed*0.15f);
            float offset1 = glm::cos(elapsed * 0.1234f) + glm::cos(elapsed/3.0f);
            
            for (auto& pair : renderables)
            {
                auto terrainRenderable = pair.second.get();

                glm::vec3 aabb_min;
                glm::vec3 aabb_max;
                aabb_min = terrainRenderable->GetMin();
                aabb_max = terrainRenderable->GetMax();

                terrainRenderable->SetWindOffsets(offset0, offset1);
                terrainRenderable->AddElapsedTime(dt);

                if (frustum.IsBoxVisible(aabb_min, aabb_max))
                {
                    terrainRenderable->Render(*renderer, cam.GetViewMatrix(), proj);
                }
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


void InitializeWindow(int windowW, int windowH)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL2 video subsystem couldn't be initialized. Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow(
                            "Procedural Grass",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            windowW, windowH,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_SetWindowResizable(window, (SDL_bool)true);


    gl_context = SDL_GL_CreateContext(window);


    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize the OpenGL context." << std::endl;
        exit(1);
    }

    std::cout << "OpenGL version loaded: " << GLVersion.major << "." << GLVersion.minor << std::endl;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 150");
}

bool DetermineTerrainSettings(const ImGuiIO& io)
{


    bool terrain_settings_determined = false;
    while (!terrain_settings_determined)
    {
        input->Update();
        if (!running) return false;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));
        ImGui::Begin("Terrain Settings");
        {

            ImGui::InputInt("Chunk Size", &settings.chunk_dimensions, 2, 64);
            ImGui::InputInt("Generation Distance", &settings.generation_range, 2, 4);
            ImGui::InputInt("Heightmap Scale", &settings.max_terrain_height, 32, 64);

            if (ImGui::Button("Generate Terrain"))
            {
                terrain_settings_determined = true;
            }
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    return true;
}


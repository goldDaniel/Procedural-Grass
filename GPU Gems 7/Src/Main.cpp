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


#include "Terrain/GrassMesh.h"
#include "Terrain/TerrainMeshGenerator.h"

struct TerrainSettings
{
    int chunk_dimensions = 32;
    int size = 32;
    int max_terrain_height = 512;
};
TerrainSettings settings;


std::unique_ptr<Renderer> renderer;
std::unique_ptr<Input> input;

SDL_Window* window;
SDL_GLContext gl_context;

bool running = true;

void InitializeWindow(int windowW, int windowH);
bool DetermineTerrainSettings(const ImGuiIO& io);

Renderable* GenerateChunkRenderable(TerrainMesh const* const chunk, Shader* const shader, Texture2D* const grass, glm::mat4 const& model);

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

    Camera cam;
    std::vector<Renderable*> terrainRenderables;

    Shader* shader = renderer->CreateShader("Assets/Shaders/VertexShader.glsl", "Assets/Shaders/FragmentShader.glsl");
    Texture2D* grass = renderer->CreateTexture2D("Assets/Textures/Ground/Grass.jpg");

    
    TerrainChunkGenerator gen(settings.chunk_dimensions, settings.max_terrain_height, 4.f);
    std::vector<std::shared_ptr<TerrainMesh>> chunks = gen.GenerateChunkSet(settings.size);
    
    std::vector<glm::mat4> transforms;
    std::vector<glm::vec3> normals;

    for (auto chunk : chunks)
    {        
        glm::mat4 model(1.0);
        model = glm::translate(model, glm::vec3(chunk->chunkX * settings.chunk_dimensions, 0, chunk->chunkZ * settings.chunk_dimensions));

        Renderable* renderable = GenerateChunkRenderable(chunk.get(), shader, grass, model);
        terrainRenderables.push_back(renderable);
        

        int offsetX = chunk->chunkX * settings.chunk_dimensions;
        int offsetZ = chunk->chunkZ * settings.chunk_dimensions;
        auto processPosition = [&transforms, &offsetX, &offsetZ](glm::vec3& position)
        {
            glm::mat4 trans = glm::translate(glm::mat4(1.f), glm::vec3( position.x + offsetX,
                                                                        position.y,
                                                                        position.z + offsetZ));

            float percent = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);


            float rotation = glm::pi<float>() * percent;
            float scale = 1 + percent * 8;

            trans = glm::rotate(trans, rotation, glm::vec3(0, 1, 0));
            trans = glm::scale(trans, glm::vec3(scale, scale, scale));
            trans = glm::translate(trans, glm::vec3(0, 1, 0));

            transforms.push_back(trans);
        };

        std::for_each(chunk->positions.begin(), chunk->positions.end(), processPosition);
        normals.insert(normals.end(), chunk->normals.begin(), chunk->normals.end());
    }    
    chunks.clear();
    chunks.shrink_to_fit(); 

    Shader* grassShader = new Shader("Assets/Shaders/GrassVertex.glsl", "Assets/Shaders/GrassFragment.glsl");
    Renderable* instancedRenderable;
    {
        GrassMesh mesh;

        std::vector<glm::vec3> scales;
        for (size_t i = 0; i < normals.size(); i++)
        {
            float scaleX = 2 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.f;
            float scaleY = 2 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.f;
            float scaleZ = 2 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.f;
            scales.push_back(glm::vec3(scaleX, scaleY, scaleZ));
        }

        VertexArray* vArray = new VertexArray();
        vArray->AddVertexBuffer(&mesh.positions[0], sizeof(float) * mesh.positions.size(), 3);
        vArray->AddVertexBuffer(&mesh.texCoords[0], sizeof(float) * mesh.texCoords.size(), 2);

        vArray->AddInstancedVertexBuffer(&normals[0].x, sizeof(glm::vec3) * normals.size(), 3, 1);
        vArray->AddInstancedVertexBuffer(transforms);
        vArray->SetNumInstances(static_cast<uint32_t>(transforms.size()));

        IndexBuffer* iBuffer = new IndexBuffer(mesh.indices);

        Texture2D* grass_bilboard = renderer->CreateTexture2D("Assets/Textures/Ground/Grass_Bilboard.png");

        instancedRenderable = new Renderable(grassShader, vArray, iBuffer);
        instancedRenderable->AddTexture("grass_bilboard", grass_bilboard);

        instancedRenderable->SetModelMatrix(glm::mat4(1.f));
        instancedRenderable->SetRenderBackface(true);
        
        scales.clear();
        scales.shrink_to_fit();
    }
    transforms.clear();
    transforms.shrink_to_fit();
    normals.clear();
    normals.shrink_to_fit();

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
    
    while(running) 
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

        static bool render_grass = true;
        static bool render_terrain = true;
        static bool render_skybox = true;

        ImGui::Begin("Debug");
        {

            ImGui::Checkbox("Render Grass", &render_grass);
            ImGui::Checkbox("Render Terrain", &render_terrain);
            ImGui::Checkbox("Render Skybox", &render_skybox);
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
        

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glm::mat4 skyboxView = glm::mat4(glm::mat3(cam.GetViewMatrix()));
        glm::mat4 proj = glm::perspective(glm::pi<float>() / 4.0f, (float)w / (float)h, 1.0f, 10000.f);

        if (render_skybox)
        {
            renderer->RenderSkybox(skyboxView, proj);
        }
        
        if (render_terrain)
        {
            shader->Bind();
            shader->SetFloat("time", elapsed);
            for (Renderable* r : terrainRenderables)
            {
                r->SetViewMatrix(cam.GetViewMatrix());
                r->SetProjectionMatrix(proj);

                renderer->Render(*r);
            }
        }

        if (render_grass)
        {
            grassShader->Bind();
            grassShader->SetVec3("camPos", cam.Position);
            grassShader->SetFloat("windOffset0", glm::sin(elapsed + 3.1415f) * glm::sin(elapsed + 1.618f));
            grassShader->SetFloat("windOffset1", glm::cos(elapsed + 1.618f));
            instancedRenderable->SetViewMatrix(cam.GetViewMatrix());
            instancedRenderable->SetProjectionMatrix(proj);
            renderer->Render(*instancedRenderable);
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
            ImGui::InputInt("Number of Chunks", &settings.size, 2, 64);
            ImGui::InputInt("Heightmap Scale", &settings.max_terrain_height, 32, 1024);

            if (ImGui::Button("Generate Terrain"))
            {
                terrain_settings_determined = true;
            }
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    return true;
}

Renderable* GenerateChunkRenderable(TerrainMesh const * const chunk, Shader * const shader, Texture2D * const grass, glm::mat4 const & model)
{
    VertexArray* vertexArray = renderer->CreateVertexArray();

    vertexArray->AddVertexBuffer(&chunk->positions[0].x, sizeof(glm::vec3) * chunk->positions.size(), 3);
    vertexArray->AddVertexBuffer(&chunk->normals[0].x, sizeof(glm::vec3) * chunk->normals.size(), 3);
    vertexArray->AddVertexBuffer(&chunk->texCoords[0].x, sizeof(glm::vec2) * chunk->texCoords.size(), 2);

    IndexBuffer* iBuffer = renderer->CreateIndexBuffer(chunk->indices);

    Renderable* renderable = renderer->CreateRenderable(shader, vertexArray, iBuffer);
    renderable->AddTexture("grass", grass);


    renderable->SetModelMatrix(model);

    return renderable;
}
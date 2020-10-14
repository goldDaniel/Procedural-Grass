#include <iostream>
#include <chrono>
#include <vector>


#define STB_IMAGE_IMPLEMENTATION
#include "Vendor/stb_image/stb_image.h"


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <SDL.h>

#include "Renderer/Texture.h"
#include "Renderer/Renderer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Camera.h"
#include "Input.h"


#include "Terrain/TerrainMeshGenerator.h"

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL2 video subsystem couldn't be initialized. Error: "
            << SDL_GetError()
            << std::endl;
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow(
        "Procedural Grass",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN |
        SDL_WINDOW_OPENGL);
    SDL_SetWindowResizable(window, (SDL_bool)true);


    SDL_GLContext gl_context = SDL_GL_CreateContext(window);


    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize the OpenGL context." << std::endl;
        exit(1);
    }

    std::cout << "OpenGL version loaded: " << GLVersion.major << "." << GLVersion.minor << std::endl;




    Camera cam;
    Renderer renderer;
    std::vector<Renderable*> terrainRenderables;

    Shader* shader = renderer.CreateShader("Assets/Shaders/VertexShader.glsl", "Assets/Shaders/FragmentShader.glsl");
    Texture2D* grass = Texture2D::CreateTexture("Assets/Textures/Ground/Grass.jpg");
    Texture2D* grass_bilboard = Texture2D::CreateTexture("Assets/Textures/Ground/Grass_Bilboard.png");

    int chunkDimensions = 32;
    int size = 32;
    float count = 0;
    float total = size * size * 2;


    Shader* instanceShader = new Shader("Assets/Shaders/InstancingTestVertex.glsl","Assets/Shaders/InstancingTestFragment.glsl");
    std::vector<float> vertices =
    {
        -1.0, -1.0, 0.0,
         1.0, -1.0, 0.0,
         0.0,  1.0, 0.0,
    };
    std::vector<unsigned int> indices =
    {
        0, 1, 2,
    };

    std::vector<float> offsets;
    int numOffsets = 128;
    for (int i = 0; i < numOffsets; i++)
    {
        for (int j = 0; j < numOffsets; j++)
        {
            offsets.push_back(i * 2);
            offsets.push_back(j * 2);
            offsets.push_back(0);
        }
    }

    VertexArray* instanceVArray = new VertexArray(numOffsets * numOffsets);
    instanceVArray->AddVertexBuffer(vertices, 3);
    instanceVArray->AddInstancedVertexBuffer(offsets, 3, 1);
    IndexBuffer* instanceIBuffer = new IndexBuffer(indices);
    Renderable* instanceRenderable = new Renderable(instanceShader, instanceVArray, instanceIBuffer);
    instanceRenderable->SetModelMatrix(glm::mat4(1.0));

    TerrainChunkGenerator gen(chunkDimensions, 256, 8.F);
    std::cout << "Beginning terrain generation..." << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<TerrainMesh*> chunks = gen.GenerateChunkSet(size);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds > (t2 - t1).count();
    std::cout << "Finished terrian generation. Time: " << duration << "ms" << std::endl;

    for (TerrainMesh* chunk : chunks)
    {
        VertexArray* vertexArray = renderer.CreateVertexArray();
        vertexArray->AddVertexBuffer(chunk->positions, 3);
        vertexArray->AddVertexBuffer(chunk->normals, 3);
        vertexArray->AddVertexBuffer(chunk->texCoords, 2);

        IndexBuffer* iBuffer = renderer.CreateIndexBuffer(chunk->indices);

        Renderable* renderable = renderer.CreateRenderable(shader, vertexArray, iBuffer); 
        renderable->AddTexture("grass", grass);

        glm::mat4 model(1.0);
        model = glm::translate(model, glm::vec3(chunk->chunkX * chunkDimensions, 0, chunk->chunkZ * chunkDimensions));
        renderable->SetModelMatrix(model);

        terrainRenderables.push_back(renderable);

        delete chunk;
    }    
    chunks.clear();
    chunks.shrink_to_fit(); 

    bool running = true;
    Input input([&running]() { running = false; });
    

    uint32_t prevTime = SDL_GetTicks();
    glm::vec2 prevMousePos = input.GetMousePos();

    float time = 0;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    while(running) 
    {
        uint32_t currentTime = SDL_GetTicks();
        float dt = static_cast<float>(currentTime - prevTime) / 1000.f;
        prevTime = currentTime;
        time += dt / 8.0f;

        input.Update();

      
        glm::vec2 mousePos = input.GetMousePos();
        glm::vec2 mouseDelta = mousePos - prevMousePos;
        prevMousePos = mousePos;

        if (input.IsButtonDown(SDL_BUTTON_LEFT))
        {
            cam.ProcessMouseMovement(mouseDelta.x, mouseDelta.y);
        }
        float speed = 64;
        if (input.IsKeyDown(SDLK_a)) cam.ProcessKeyboard(Camera_Movement::LEFT, dt);
        if (input.IsKeyDown(SDLK_d)) cam.ProcessKeyboard(Camera_Movement::RIGHT, dt);
        
        if (input.IsKeyDown(SDLK_w)) cam.ProcessKeyboard(Camera_Movement::FORWARD, dt);
        if (input.IsKeyDown(SDLK_s)) cam.ProcessKeyboard(Camera_Movement::BACKWARD, dt);
        
        if (input.IsKeyDown(SDLK_LSHIFT)) cam.ProcessKeyboard(Camera_Movement::DOWN, dt);
        if (input.IsKeyDown(SDLK_SPACE)) cam.ProcessKeyboard(Camera_Movement::UP, dt);
        

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        glViewport(0, 0, w, h);


        glm::mat4 skyboxView = glm::mat4(glm::mat3(cam.GetViewMatrix()));
        glm::mat4 proj = glm::perspective(glm::pi<float>() / 4.0f, (float)w / (float)h, 1.0f, 4000.f);

        renderer.RenderSkybox(skyboxView, proj);
        
        shader->Bind();
        shader->SetFloat("time", time);

        
        for (Renderable* r : terrainRenderables)
        {
            r->SetViewMatrix(cam.GetViewMatrix());
            r->SetProjectionMatrix(proj);

            renderer.Render(*r);
        }

        instanceRenderable->SetViewMatrix(cam.GetViewMatrix());
        instanceRenderable->SetProjectionMatrix(proj);
        renderer.Render(*instanceRenderable);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
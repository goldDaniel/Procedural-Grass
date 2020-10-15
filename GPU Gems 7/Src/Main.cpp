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

#include "Renderer/Texture.h"
#include "Renderer/Renderer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Camera.h"
#include "Input.h"


#include "Terrain/GrassMesh.h"
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
    

    int chunkDimensions = 16;
    int size = 32;
    TerrainChunkGenerator gen(chunkDimensions, 512, 2.f);
    std::cout << "Beginning terrain generation..." << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<TerrainMesh*> chunks = gen.GenerateChunkSet(size);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds > (t2 - t1).count();
    std::cout << "Finished terrian generation. Time: " << duration << "ms" << std::endl;

    std::vector<glm::mat4> transforms;
    std::vector<glm::vec3> normals;

    for (TerrainMesh* chunk : chunks)
    {        
        VertexArray* vertexArray = renderer.CreateVertexArray();

        

        vertexArray->AddVertexBuffer(&chunk->positions[0].x, sizeof(glm::vec3) * chunk->positions.size(), 3);
        vertexArray->AddVertexBuffer(&chunk->normals[0].x, sizeof(glm::vec3) * chunk->normals.size(), 3);
        vertexArray->AddVertexBuffer(&chunk->texCoords[0].x, sizeof(glm::vec2) * chunk->texCoords.size(), 2);

        IndexBuffer* iBuffer = renderer.CreateIndexBuffer(chunk->indices);

        Renderable* renderable = renderer.CreateRenderable(shader, vertexArray, iBuffer); 
        renderable->AddTexture("grass", grass);

        glm::mat4 model(1.0);
        model = glm::translate(model, glm::vec3(chunk->chunkX * chunkDimensions, 0, chunk->chunkZ * chunkDimensions));
        renderable->SetModelMatrix(model);

        terrainRenderables.push_back(renderable);

        int offsetX = chunk->chunkX * chunkDimensions;
        int offsetZ = chunk->chunkZ * chunkDimensions;
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

        delete chunk;
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
        vArray->SetNumInstances(transforms.size());

        IndexBuffer* iBuffer = new IndexBuffer(mesh.indices);

        Texture2D* grass_bilboard = Texture2D::CreateTexture("Assets/Textures/Ground/Grass_Bilboard.png");

        instancedRenderable = new Renderable(grassShader, vArray, iBuffer);
        instancedRenderable->AddTexture("grass_bilboard", grass_bilboard);

        instancedRenderable->SetModelMatrix(glm::mat4(1.f));
        
        scales.clear();
        scales.shrink_to_fit();
    }
    transforms.clear();
    transforms.shrink_to_fit();
    normals.clear();
    normals.shrink_to_fit();


   
    bool running = true;
    Input input([&running]() { running = false; });
    

    uint32_t prevTime = SDL_GetTicks();
    glm::vec2 prevMousePos = input.GetMousePos();

    float elapsed = 0;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    while(running) 
    {
        uint32_t currentTime = SDL_GetTicks();
        float dt = static_cast<float>(currentTime - prevTime) / 1000.f;
        prevTime = currentTime;
        elapsed += dt;

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
        glm::mat4 proj = glm::perspective(glm::pi<float>() / 4.0f, (float)w / (float)h, 1.0f, 10000.f);

        renderer.RenderSkybox(skyboxView, proj);
        
        shader->Bind();
        shader->SetFloat("time", elapsed);

        
        for (Renderable* r : terrainRenderables)
        {
            r->SetViewMatrix(cam.GetViewMatrix());
            r->SetProjectionMatrix(proj);

            renderer.Render(*r);
        }

        glDisable(GL_CULL_FACE);
        grassShader->Bind();
        grassShader->SetFloat("windOffset0", glm::sin(elapsed + 3.1415f)* glm::sin(elapsed + 1.618f));
        grassShader->SetFloat("windOffset1", glm::cos(elapsed + 1.618f));
        instancedRenderable->SetViewMatrix(cam.GetViewMatrix());
        instancedRenderable->SetProjectionMatrix(proj);
        renderer.Render(*instancedRenderable);
        glEnable(GL_CULL_FACE);


        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
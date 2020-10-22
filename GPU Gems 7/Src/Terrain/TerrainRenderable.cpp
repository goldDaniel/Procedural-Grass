#include "TerrainRenderable.h"

#include "GrassMesh.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

TerrainRenderable::TerrainRenderable(const TerrainMesh& mesh, Renderer& renderer)
{
    offset0 = offset1 = 0;
    elapsed_time = 0;

    aabb_min.x = static_cast<float>(mesh.chunkX * mesh.chunk_dimensions - mesh.chunk_dimensions / 2);
    aabb_max.x = static_cast<float>(mesh.chunkX * mesh.chunk_dimensions + mesh.chunk_dimensions / 2);

    aabb_min.y = static_cast<float>(-mesh.chunk_height);
    aabb_max.y = static_cast<float>(mesh.chunk_height);

    aabb_min.z = static_cast<float>(mesh.chunkZ * mesh.chunk_dimensions - mesh.chunk_dimensions / 2);
    aabb_max.z = static_cast<float>(mesh.chunkZ * mesh.chunk_dimensions + mesh.chunk_dimensions / 2);

    std::vector<glm::mat4> grass_transforms;
    GenerateGroundRenderable(renderer, mesh, grass_transforms);
    GenerateGrassRenderable(renderer, grass_transforms, mesh.normals);
}

void TerrainRenderable::Render(const Renderer& renderer, glm::mat4 const& view, glm::mat4 const& proj)
{
    heightmap_shader->Bind();
    heightmap_renderable->SetViewMatrix(view);
    heightmap_renderable->SetProjectionMatrix(proj);

    renderer.Render(*heightmap_renderable);

    grass_shader->Bind();
    grass_shader->SetFloat("windOffset0", offset0);
    grass_shader->SetFloat("windOffset1", offset1);
    grass_shader->SetFloat("terrain_scale", this->terrain_scale);

    grass_blades_renderable->SetViewMatrix(view);
    grass_blades_renderable->SetProjectionMatrix(proj);

    renderer.Render(*grass_blades_renderable);
}

void TerrainRenderable::SetWindOffsets(float offset0, float offset1)
{
    this->offset0 = offset0;
    this->offset1 = offset1;
}

void TerrainRenderable::SetTerrainScale(float scale)
{
    this->terrain_scale = scale;
}

void TerrainRenderable::AddElapsedTime(float dt)
{
    this->elapsed_time += dt;
}

glm::vec3 TerrainRenderable::GetMin()
{
    return aabb_min;
}

glm::vec3 TerrainRenderable::GetMax()
{
    return aabb_max;
}


void TerrainRenderable::GenerateGroundRenderable(Renderer& renderer,
                                                 TerrainMesh const& chunk,
                                                 std::vector<glm::mat4>& transforms)
{
    heightmap_shader = renderer.CreateShader("Assets/Shaders/VertexShader.glsl", "Assets/Shaders/FragmentShader.glsl");
    Texture2D* ground_texture = renderer.CreateTexture2D("Assets/Textures/Ground/Grass.jpg");

    VertexArray* vertexArray = renderer.CreateVertexArray();
    vertexArray->AddVertexBuffer(&chunk.positions[0].x, sizeof(glm::vec3) * chunk.positions.size(), 3);
    vertexArray->AddVertexBuffer(&chunk.normals[0].x, sizeof(glm::vec3) * chunk.normals.size(), 3);
    vertexArray->AddVertexBuffer(&chunk.texCoords[0].x, sizeof(glm::vec2) * chunk.texCoords.size(), 2);

    IndexBuffer* iBuffer = renderer.CreateIndexBuffer(chunk.indices);

    heightmap_renderable = renderer.CreateRenderable(heightmap_shader, vertexArray, iBuffer);
    heightmap_renderable->AddTexture("grass", ground_texture);
    heightmap_renderable->SetModelMatrix(glm::mat4(1.f));

    int offsetX = chunk.chunkX * chunk.chunk_dimensions;
    int offsetZ = chunk.chunkZ * chunk.chunk_dimensions;
    for (size_t i = 0; i < chunk.positions.size(); i++)
    {
        glm::mat4 trans = glm::translate(glm::mat4(1.f), chunk.positions[i]);

        float percent = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);


        float rotation = glm::pi<float>() * percent;
        float scale = 1 + percent * 8;

        trans = glm::rotate(trans, rotation, glm::vec3(0, 1, 0));
        trans = glm::scale(trans, glm::vec3(scale, scale, scale));
        trans = glm::translate(trans, glm::vec3(0, 1, 0));


        transforms.push_back(trans);
    }
}

void TerrainRenderable::GenerateGrassRenderable(Renderer& renderer, const std::vector<glm::mat4>& transforms, const std::vector<glm::vec3>& normals)
{
    grass_shader = renderer.CreateShader("Assets/Shaders/GrassVertex.glsl", "Assets/Shaders/GrassFragment.glsl");
    Texture2D* grass_bilboard = renderer.CreateTexture2D("Assets/Textures/Ground/Grass_Bilboard.png");

    GrassMesh mesh;

    VertexArray* vArray = new VertexArray();
    vArray->AddVertexBuffer(&mesh.positions[0], sizeof(float) * mesh.positions.size(), 3);
    vArray->AddVertexBuffer(&mesh.texCoords[0], sizeof(float) * mesh.texCoords.size(), 2);

    vArray->AddInstancedVertexBuffer(&normals[0].x, sizeof(glm::vec3) * normals.size(), 3, 1);
    vArray->AddInstancedVertexBuffer(transforms);
    vArray->SetNumInstances(static_cast<uint32_t>(transforms.size()));

    IndexBuffer* iBuffer = new IndexBuffer(mesh.indices);

    grass_blades_renderable = renderer.CreateRenderable(grass_shader, vArray, iBuffer);
    grass_blades_renderable->AddTexture("grass_bilboard", grass_bilboard);

    grass_blades_renderable->SetModelMatrix(glm::mat4(1.f));
    grass_blades_renderable->SetRenderBackface(true);
}

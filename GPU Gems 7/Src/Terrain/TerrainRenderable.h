#ifndef TERRAIN_RENDERABLE_H_
#define TERRAIN_RENDERABLE_H_

#include "Renderer/Renderable.h"
#include "TerrainMesh.h"

class TerrainRenderable
{
public:
	TerrainRenderable(const TerrainMesh& mesh, Renderer& renderer)
	{
        offset0 = offset1 = 0;
        elapsed_time = 0;
        
        std::vector<glm::mat4> grass_transforms;
        std::vector<glm::vec3> grass_normals;
        GenerateGroundRenderable(renderer, mesh, grass_transforms, grass_normals, glm::mat4(1.f));
        GenerateGrassRenderable(renderer, grass_transforms, grass_normals);
	}

    void Render(const Renderer& renderer, glm::mat4 const & view, glm::mat4 const& proj)
    {
        heightmap_shader->Bind();
        heightmap_renderable->SetViewMatrix(view);
        heightmap_renderable->SetProjectionMatrix(proj);

        renderer.Render(*heightmap_renderable);
        
        grass_shader->Bind(); 
        grass_shader->SetFloat("windOffset0", offset0);
        grass_shader->SetFloat("windOffset1", offset1);
        
        grass_blades_renderable->SetViewMatrix(view);
        grass_blades_renderable->SetProjectionMatrix(proj); 

        renderer.Render(*grass_blades_renderable);
    }

    void SetWindOffsets(float offset0, float offset1)
    {
        this->offset0 = offset0;
        this->offset1 = offset1;
    }

    void AddElapsedTime(float dt)
    {
        this->elapsed_time += dt;
    } 

private:

    float offset0;
    float offset1;

    float elapsed_time;

    Shader* heightmap_shader;
    Shader* grass_shader;

	Renderable* heightmap_renderable;
	Renderable* grass_blades_renderable;

    void GenerateGroundRenderable(Renderer& renderer, 
                                  TerrainMesh const&chunk, 
                                  std::vector<glm::mat4>& transforms,
                                  std::vector<glm::vec3>& normals,
                                  glm::mat4 const& model)
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
        heightmap_renderable->SetModelMatrix(model);

        int offsetX = chunk.chunkX * chunk.chunk_dimensions;
        int offsetZ = chunk.chunkZ * chunk.chunk_dimensions;
        auto processPosition = [&transforms, &offsetX, &offsetZ](glm::vec3 position)
        {
            glm::mat4 trans = glm::translate(glm::mat4(1.f), glm::vec3(position.x,
                                                                       position.y,
                                                                       position.z));

            float percent = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);


            float rotation = glm::pi<float>() * percent;
            float scale = 1 + percent * 8;

            trans = glm::rotate(trans, rotation, glm::vec3(0, 1, 0));
            trans = glm::scale(trans, glm::vec3(scale, scale, scale));
            trans = glm::translate(trans, glm::vec3(0, 1, 0));

            transforms.push_back(trans);
        };
        std::for_each(chunk.positions.begin(), chunk.positions.end(), processPosition);
        normals.insert(normals.end(), chunk.normals.begin(), chunk.normals.end());
    }

    void GenerateGrassRenderable(Renderer& renderer, const std::vector<glm::mat4>& transforms, const std::vector<glm::vec3>& normals)
    {
        grass_shader = renderer.CreateShader("Assets/Shaders/GrassVertex.glsl", "Assets/Shaders/GrassFragment.glsl");
        Texture2D* grass_bilboard = renderer.CreateTexture2D("Assets/Textures/Ground/Grass_Bilboard.png");

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

        grass_blades_renderable = renderer.CreateRenderable(grass_shader, vArray, iBuffer);
        grass_blades_renderable->AddTexture("grass_bilboard", grass_bilboard);

        grass_blades_renderable->SetModelMatrix(glm::mat4(1.f));
        grass_blades_renderable->SetRenderBackface(true);

        
    }
};


#endif
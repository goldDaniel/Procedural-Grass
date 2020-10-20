#ifndef TERRAIN_RENDERABLE_H_
#define TERRAIN_RENDERABLE_H_

#include "Renderer/Renderer.h"
#include "TerrainMesh.h"

class TerrainRenderable
{
public:
    TerrainRenderable(const TerrainMesh& mesh, Renderer& renderer);
    ~TerrainRenderable() {}

    void Render(const Renderer& renderer, glm::mat4 const& view, glm::mat4 const& proj);

    void SetWindOffsets(float offset0, float offset1);
    void AddElapsedTime(float dt);

    glm::vec3 GetMin();
    glm::vec3 GetMax();

private:

    float offset0;
    float offset1;

    float elapsed_time;

    glm::vec3 aabb_min;
    glm::vec3 aabb_max;

    Shader* heightmap_shader;
    Shader* grass_shader;

	Renderable* heightmap_renderable;
	Renderable* grass_blades_renderable;

    

    void GenerateGroundRenderable(Renderer& renderer,
                                  TerrainMesh const& chunk,
                                  std::vector<glm::mat4>& transforms);

    void GenerateGrassRenderable(Renderer& renderer, const std::vector<glm::mat4>& transforms, const std::vector<glm::vec3>& normals);
};


#endif
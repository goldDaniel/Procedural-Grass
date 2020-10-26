#pragma once
#ifndef TERRAIN_CHUNK_MESH_H
#define TERRAIN_CHUNK_MESH_H

#include <vector>
#include <glm/glm.hpp>

struct TerrainMesh
{
	int chunkX;
	int chunkZ;

	int chunk_dimensions;
	
	int chunk_height;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	std::vector<unsigned int> indices;
};



#endif
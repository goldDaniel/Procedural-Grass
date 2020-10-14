#pragma once
#ifndef TERRAIN_CHUNK_MESH_H
#define TERRAIN_CHUNK_MESH_H

#include <vector>
#include <glm/glm.hpp>

struct TerrainMesh
{
	int chunkX;
	int chunkZ;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	std::vector<unsigned int> indices;

	~TerrainMesh()
	{
		positions.clear();
		positions.shrink_to_fit();

		normals.clear();
		normals.shrink_to_fit();

		texCoords.clear();
		texCoords.shrink_to_fit();

		indices.clear();
		indices.shrink_to_fit();
	}
};



#endif
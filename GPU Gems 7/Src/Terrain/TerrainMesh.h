#pragma once
#ifndef TERRAIN_CHUNK_MESH_H
#define TERRAIN_CHUNK_MESH_H

#include <vector>

struct TerrainMesh
{
	int chunkX;
	int chunkZ;

	std::vector<float> positions;
	std::vector<float> normals; 
	std::vector<float> texCoords;

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
#pragma once

#ifndef TERRAIN_CHUNK_GEN_H_
#define TERRAIN_CHUNK_GEN_H_

#include "TerrainMesh.h"

class TerrainChunkGenerator
{
public: 

	TerrainChunkGenerator(int chunkDimensions, int maxHeight = 128, float scale = 1);
	~TerrainChunkGenerator();

	std::vector<TerrainMesh*> GenerateChunkSet(int size) const;

	

private:
	const float scale;
	const int chunkDimensions;
	const int maxHeight;
};

#endif
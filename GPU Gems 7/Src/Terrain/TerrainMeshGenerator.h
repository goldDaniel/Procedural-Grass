#pragma once

#ifndef TERRAIN_CHUNK_GEN_H_
#define TERRAIN_CHUNK_GEN_H_

#include "TerrainMesh.h"
#include <memory>
#include <unordered_map>


using Coords = std::pair<int, int>;

class TerrainChunkGenerator
{
public: 

	TerrainChunkGenerator(int chunkDimensions, int maxHeight = 128, float scale = 1);
	~TerrainChunkGenerator();

	std::shared_ptr<TerrainMesh> GenerateChunk(int chunkX, int chunkZ);

	bool HasGenerated(int chunkX, int chunkZ) const 
	{
		return std::find(hasGenerated.begin(), hasGenerated.end(), Coords(chunkX, chunkZ)) != hasGenerated.end();
	}

	void Unload(int chunkX, int chunkZ)
	{
		assert(std::find(hasGenerated.begin(), hasGenerated.end(), Coords(chunkX, chunkZ) != hasGenerated.end()));

		auto pos = std::find(hasGenerated.begin(), hasGenerated.end(), Coords(chunkX, chunkZ));
		hasGenerated.erase(pos);
	}

private:
	const float scale;
	const int chunkDimensions;
	const int maxHeight;

	const int generationRange = 16;

	std::vector<Coords> hasGenerated;
};

#endif
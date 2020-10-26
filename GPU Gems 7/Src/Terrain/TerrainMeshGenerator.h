#pragma once

#ifndef TERRAIN_CHUNK_GEN_H_
#define TERRAIN_CHUNK_GEN_H_

#include "TerrainMesh.h"
#include "TerrainSettings.h"
#include <memory>
#include <unordered_map>


using Coords = std::pair<int, int>;

class TerrainChunkGenerator
{
public: 

	TerrainChunkGenerator(TerrainSettings settings);
	~TerrainChunkGenerator();

	TerrainMesh* GenerateChunk(int chunkX, int chunkZ);

	bool HasGenerated(int chunkX, int chunkZ) const 
	{
		return std::find(has_generated.begin(), has_generated.end(), Coords(chunkX, chunkZ)) != has_generated.end();
	}

	void Unload(int chunkX, int chunkZ)
	{
		auto pos = std::find(has_generated.begin(), has_generated.end(), Coords(chunkX, chunkZ));
		has_generated.erase(pos);
	}

private:
	TerrainSettings settings;

	std::vector<Coords> has_generated;

	std::unique_ptr<TerrainMesh> mesh;
};

#endif
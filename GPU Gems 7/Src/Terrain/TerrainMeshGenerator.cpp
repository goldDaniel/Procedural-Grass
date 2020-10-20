#include "TerrainMeshGenerator.h"

#include <glm/glm.hpp>
#include <future>

#include <iostream>
#include <memory>

#include "Vendor/fastnoise/FastNoise.h"

static std::shared_ptr<TerrainMesh> GenerateChunkMesh(FastNoise noise, int chunkX, int chunkZ, int chunkDimensions, int maxHeight, float scale);



TerrainChunkGenerator::TerrainChunkGenerator(int chunkDimensions, int maxHeight, float scale)
	: chunkDimensions(chunkDimensions), maxHeight(maxHeight), scale(scale)
{
}

TerrainChunkGenerator::~TerrainChunkGenerator()
{

}

std::shared_ptr<TerrainMesh> TerrainChunkGenerator::GenerateChunk(int chunkX, int chunkZ)
{
	FastNoise noise;
	noise.SetFrequency(0.003);
	noise.SetFractalOctaves(8);

	hasGenerated.push_back(Coords(chunkX, chunkZ));

	return GenerateChunkMesh(noise, chunkX, chunkZ, chunkDimensions, maxHeight, scale);
}



static std::shared_ptr<TerrainMesh> GenerateChunkMesh(const FastNoise noise, int chunkX, int chunkZ, int chunkDimensions, int maxHeight, float scale)
{
	std::shared_ptr<TerrainMesh> result = std::make_shared<TerrainMesh>();

	result->chunkX = static_cast<int>(chunkX * scale);
	result->chunkZ = static_cast<int>(chunkZ * scale);
	result->chunk_dimensions = chunkDimensions + 1;
	result->chunk_height = maxHeight;
	result->positions.reserve(3 * static_cast<size_t>(chunkDimensions) * static_cast<size_t>(chunkDimensions));
	result->normals.reserve(3 * static_cast<size_t>(chunkDimensions) * static_cast<size_t>(chunkDimensions));
	result->texCoords.reserve(2 * static_cast<size_t>(chunkDimensions) * static_cast<size_t>(chunkDimensions));

	int padding = 1;

	float finalChunkSize = (chunkDimensions + padding) / 2.f;

	float startX = -finalChunkSize;
	float startZ = -finalChunkSize;
	float endX   = finalChunkSize;
	float endZ =   finalChunkSize;


	for (float x = startX; x < endX; x++)
	{
		for (float z = startZ; z < endZ; z++)
		{
			float y0 = noise.GetValue(chunkX * chunkDimensions + x, chunkZ * chunkDimensions + z) * maxHeight;
			float y1 = noise.GetValue(chunkX * chunkDimensions + x, chunkZ * chunkDimensions + z + 1) * maxHeight;
			float y2 = noise.GetValue(chunkX * chunkDimensions + x + 1, chunkZ * chunkDimensions + z + 1) * maxHeight;

			glm::vec3 pos0((chunkX * chunkDimensions + x) * scale, y0, (chunkZ * chunkDimensions + z) * scale);
			glm::vec3 pos1((chunkX * chunkDimensions + x) * scale, y1, (chunkZ * chunkDimensions + z + 1) * scale);
			glm::vec3 pos2((chunkX * chunkDimensions + x + 1) * scale, y2, (chunkZ * chunkDimensions + z + 1) * scale);

			glm::vec3 normal = glm::normalize(cross(pos1 - pos0, pos2 - pos0));

			glm::vec2 texCoord = 
			{
				(x - startX) / (endX - startX),
				(z - startZ) / (endZ - startZ)
			};

			result->positions.push_back(pos0);
			result->normals.push_back(normal);

			result->texCoords.push_back(texCoord);
		}
	}

	chunkDimensions += padding;
	for (unsigned int i = 0; i < static_cast<unsigned int>(chunkDimensions) - 1; i++)
	{
		for (unsigned int j = 0; j < static_cast<unsigned int>(chunkDimensions) - 1; j ++)
		{
			unsigned int indexA = i * chunkDimensions + j;
			unsigned int indexB = i * chunkDimensions + j + 1;
			unsigned int indexC = (i + 1) * chunkDimensions + j + 1;
			unsigned int indexD = (i + 1) * chunkDimensions+ j;


			result->indices.push_back(indexA);
			result->indices.push_back(indexB);
			result->indices.push_back(indexC);

			result->indices.push_back(indexA);
			result->indices.push_back(indexC);
			result->indices.push_back(indexD);
		}
	}

	return result;
}
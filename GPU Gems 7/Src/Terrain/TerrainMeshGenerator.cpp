#include "TerrainMeshGenerator.h"

#include <glm/glm.hpp>
#include <future>

#include "Vendor/fastnoise/FastNoise.h"

static TerrainMesh* GenerateChunk(FastNoise noise, int chunkX, int chunkZ, int chunkDimensions, int maxHeight, float scale);



TerrainChunkGenerator::TerrainChunkGenerator(int chunkDimensions, int maxHeight, float scale)
	: chunkDimensions(chunkDimensions), maxHeight(maxHeight), scale(scale)
{
}

TerrainChunkGenerator::~TerrainChunkGenerator()
{

}

std::vector<TerrainMesh*> TerrainChunkGenerator::GenerateChunkSet(int size) const
{
	std::vector<TerrainMesh*> result;

	FastNoise noise;
	noise.SetFrequency(0.003);
	noise.SetFractalOctaves(8);

#if 1
	
	std::vector<std::future<TerrainMesh*>> futures;

	for (int x = -size; x < size; ++x)
	{
		for (int z = -size; z < size; ++z)
		{
			futures.push_back(std::async(GenerateChunk, noise, x, z, chunkDimensions, maxHeight, scale));
		}
	}

	for (size_t i = 0; i < futures.size(); ++i)
	{
		result.push_back(futures[i].get());
	}
#else 
	for (int x = -size; x < size; ++x)
	{
		for (int z = -size; z < size; ++z)
		{
			result.push_back(GenerateChunk(noise, x, z, chunkDimensions, maxHeight));
		}
	}
#endif

	return result;
}

static TerrainMesh* GenerateChunk(const FastNoise noise, int chunkX, int chunkZ, int chunkDimensions, int maxHeight, float scale)
{
	TerrainMesh* result = new TerrainMesh();

	result->chunkX = chunkX * scale;
	result->chunkZ = chunkZ * scale;
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

			glm::vec3 pos0(x * scale, y0, z * scale);
			glm::vec3 pos1(x * scale, y1, (z + 1) * scale);
			glm::vec3 pos2((x + 1) * scale, y2, (z + 1) * scale);

			glm::vec3 normal = glm::normalize(cross(pos1 - pos0, pos2 - pos0));

			float texCoordX = (x - startX) / (endX - startX);
			float texCoordY = (z - startZ) / (endZ - startZ);

			
			result->positions.push_back(pos0.x);
			result->positions.push_back(pos0.y);
			result->positions.push_back(pos0.z);

			result->normals.push_back(normal.x);
			result->normals.push_back(normal.y);
			result->normals.push_back(normal.z);

			result->texCoords.push_back(texCoordX);
			result->texCoords.push_back(texCoordY);
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
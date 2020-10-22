#include "TerrainMeshGenerator.h"

#include <glm/glm.hpp>
#include <future>

#include <iostream>
#include <memory>

#include "Vendor/fastnoise/FastNoise.h"

static std::unique_ptr<TerrainMesh> GenerateChunkMesh(FastNoise noise, int chunkX, int chunkZ, int chunkDimensions, int maxHeight, int scale);
static glm::vec3 GenerateVertexPosition(const FastNoise& noise, int x, int z, int heightScale);



TerrainChunkGenerator::TerrainChunkGenerator(int chunkDimensions, int maxHeight, int scale)
	: chunkDimensions(chunkDimensions), maxHeight(maxHeight), scale(scale)
{
}

TerrainChunkGenerator::~TerrainChunkGenerator()
{

}

std::unique_ptr<TerrainMesh> TerrainChunkGenerator::GenerateChunk(int chunkX, int chunkZ)
{
	FastNoise noise;
	noise.SetFrequency(0.003f);
	noise.SetFractalOctaves(8);

	hasGenerated.push_back(Coords(chunkX, chunkZ));

	return GenerateChunkMesh(noise, chunkX, chunkZ, chunkDimensions, maxHeight, scale);
}



static std::unique_ptr<TerrainMesh> GenerateChunkMesh(const FastNoise noise, int chunkX, int chunkZ, int chunkDimensions, int maxHeight, int scale)
{
	std::unique_ptr<TerrainMesh> result = std::make_unique<TerrainMesh>();

	result->chunkX = static_cast<int>(chunkX * scale);
	result->chunkZ = static_cast<int>(chunkZ * scale);
	result->chunk_dimensions = chunkDimensions + 1;
	result->chunk_height = maxHeight;
	result->positions.reserve(static_cast<size_t>(chunkDimensions) * static_cast<size_t>(chunkDimensions));
	result->normals.reserve(static_cast<size_t>(chunkDimensions) * static_cast<size_t>(chunkDimensions));
	result->texCoords.reserve(static_cast<size_t>(chunkDimensions) * static_cast<size_t>(chunkDimensions));

	int padding = 1;

	float finalChunkSize = (chunkDimensions + padding) / 2.f;

	float startX = -finalChunkSize;
	float startZ = -finalChunkSize;
	float endX   =  finalChunkSize;
	float endZ   =  finalChunkSize;


	for (int x = startX; x < endX; x++)
	{
		for (int z = startZ; z < endZ; z++)
		{
			float xCoord = chunkX * chunkDimensions + x;
			float zCoord = chunkZ * chunkDimensions + z;

			glm::vec3 pos0 = GenerateVertexPosition(noise, xCoord, zCoord, maxHeight);
			glm::vec3 pos1 = GenerateVertexPosition(noise, xCoord, zCoord + 1, maxHeight);
			glm::vec3 pos2 = GenerateVertexPosition(noise, xCoord + 1, zCoord + 1, maxHeight);

			pos0.x *= scale;
			pos0.z *= scale;
			pos1.x *= scale;
			pos1.z *= scale;
			pos2.x *= scale;
			pos2.z *= scale;

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

	return std::move(result);
}

static glm::vec3 GenerateVertexPosition(const FastNoise& noise, int x, int z, int heightScale)
{
	glm::vec3 result;
	result.x = x;
	result.z = z;

	float sample0 = ((noise.GetValue(x, z) + 1.f) / 2.f);
	float sample1 = ((noise.GetValue(x * 8.f, z * 8.f) + 1.f) / 2.f);
	float sample2 = ((noise.GetValue(x * 32.f, z * 32.f) + 1.f) / 2.f);

	result.y = sample0 * 0.79f + sample1 * 0.2f + sample2 * 0.01f;
	result.y *= heightScale;

	return result;
}
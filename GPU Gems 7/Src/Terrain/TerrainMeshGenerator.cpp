#include "TerrainMeshGenerator.h"

#include <glm/glm.hpp>
#include <future>

#include <iostream>
#include <memory>

#include "Vendor/fastnoise/FastNoise.h"



static void GenerateChunkMesh(TerrainMesh* mesh, FastNoise noise, int chunkX, int chunkZ, TerrainSettings settings);
static glm::vec3 GenerateVertexPosition(const FastNoise& noise, int x, int z, int heightScale);



TerrainChunkGenerator::TerrainChunkGenerator(TerrainSettings settings)
	: settings(settings)
{
	mesh = std::make_unique<TerrainMesh>();
}

TerrainChunkGenerator::~TerrainChunkGenerator()
{ 
}

TerrainMesh* TerrainChunkGenerator::GenerateChunk(int chunkX, int chunkZ)
{
	FastNoise noise;
	noise.SetFrequency(0.003f);
	noise.SetFractalOctaves(8);

	has_generated.push_back(Coords(chunkX, chunkZ));


	GenerateChunkMesh(mesh.get(), noise, chunkX, chunkZ, settings);

	return mesh.get();
}



static void GenerateChunkMesh(TerrainMesh* result, const FastNoise noise, int chunkX, int chunkZ, TerrainSettings settings)
{
	result->chunkX = static_cast<int>(chunkX * settings.scale);
	result->chunkZ = static_cast<int>(chunkZ * settings.scale);
	result->chunk_dimensions = settings.chunk_dimensions + 1;
	result->chunk_height = settings.max_terrain_height;
	result->positions.clear();
	result->normals.clear();
	result->texCoords.clear(); 
	result->indices.clear();

	int padding = 1;

	float finalChunkSize = (settings.chunk_dimensions + padding) / 2.f;

	float startX = -finalChunkSize;
	float startZ = -finalChunkSize;
	float endX   =  finalChunkSize;
	float endZ   =  finalChunkSize;


	for (int x = startX; x < endX; x++)
	{
		for (int z = startZ; z < endZ; z++)
		{
			float xCoord = chunkX * settings.chunk_dimensions + x;
			float zCoord = chunkZ * settings.chunk_dimensions + z;

			glm::vec3 pos0 = GenerateVertexPosition(noise, xCoord, zCoord, settings.max_terrain_height);
			glm::vec3 pos1 = GenerateVertexPosition(noise, xCoord, zCoord + 1, settings.max_terrain_height);
			glm::vec3 pos2 = GenerateVertexPosition(noise, xCoord + 1, zCoord + 1, settings.max_terrain_height);

			pos0.x *= settings.scale;
			pos0.z *= settings.scale;
			pos1.x *= settings.scale;
			pos1.z *= settings.scale;
			pos2.x *= settings.scale;
			pos2.z *= settings.scale;

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

	settings.chunk_dimensions += padding;
	for (unsigned int i = 0; i < static_cast<unsigned int>(settings.chunk_dimensions) - 1; i++)
	{
		for (unsigned int j = 0; j < static_cast<unsigned int>(settings.chunk_dimensions) - 1; j ++)
		{
			unsigned int indexA = i * settings.chunk_dimensions + j;
			unsigned int indexB = i * settings.chunk_dimensions + j + 1;
			unsigned int indexC = (i + 1) * settings.chunk_dimensions + j + 1;
			unsigned int indexD = (i + 1) * settings.chunk_dimensions + j;


			result->indices.push_back(indexA);
			result->indices.push_back(indexB);
			result->indices.push_back(indexC);

			result->indices.push_back(indexA);
			result->indices.push_back(indexC);
			result->indices.push_back(indexD);
		}
	} 
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
#pragma once
#ifndef GRASS_MESH_H_
#define GRASS_MESH_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GrassMesh
{
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texCoords;

	std::vector<unsigned int> indices;

	GrassMesh()
	{
		glm::vec3 quadA_0 = glm::vec4(-1, -1, 0, 1);
		glm::vec3 quadA_1 = glm::vec4( 1, -1, 0, 1);
		glm::vec3 quadA_2 = glm::vec4( 1,  1, 0, 1);
		glm::vec3 quadA_3 = glm::vec4(-1,  1, 0, 1);

		auto rotation = glm::rotate(glm::mat4(1.0), glm::radians(60.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::vec3 quadB_0 = rotation * glm::vec4(quadA_0, 1);
		glm::vec3 quadB_1 = rotation * glm::vec4(quadA_1, 1);
		glm::vec3 quadB_2 = rotation * glm::vec4(quadA_2, 1);
		glm::vec3 quadB_3 = rotation * glm::vec4(quadA_3, 1);

		rotation = glm::rotate(glm::mat4(1.0), glm::radians(120.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::vec3 quadC_0 = rotation * glm::vec4(quadA_0, 1);
		glm::vec3 quadC_1 = rotation * glm::vec4(quadA_1, 1);
		glm::vec3 quadC_2 = rotation * glm::vec4(quadA_2, 1);
		glm::vec3 quadC_3 = rotation * glm::vec4(quadA_3, 1);

		positions.push_back(quadA_0.x);
		positions.push_back(quadA_0.y);
		positions.push_back(quadA_0.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(0);

		positions.push_back(quadA_1.x);
		positions.push_back(quadA_1.y);
		positions.push_back(quadA_1.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(1);
		texCoords.push_back(0);

		positions.push_back(quadA_2.x);
		positions.push_back(quadA_2.y);
		positions.push_back(quadA_2.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(1);
		texCoords.push_back(1);

		positions.push_back(quadA_3.x);
		positions.push_back(quadA_3.y);
		positions.push_back(quadA_3.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(1);



		positions.push_back(quadB_0.x);
		positions.push_back(quadB_0.y);
		positions.push_back(quadB_0.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(0);
								
		positions.push_back(quadB_1.x);
		positions.push_back(quadB_1.y);
		positions.push_back(quadB_1.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(1);
		texCoords.push_back(0);
								
		positions.push_back(quadB_2.x);
		positions.push_back(quadB_2.y);
		positions.push_back(quadB_2.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(1);
		texCoords.push_back(1);
								
		positions.push_back(quadB_3.x);
		positions.push_back(quadB_3.y);
		positions.push_back(quadB_3.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(1);



		positions.push_back(quadC_0.x);
		positions.push_back(quadC_0.y);
		positions.push_back(quadC_0.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(0);

		positions.push_back(quadC_1.x);
		positions.push_back(quadC_1.y);
		positions.push_back(quadC_1.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(1);
		texCoords.push_back(0);

		positions.push_back(quadC_2.x);
		positions.push_back(quadC_2.y);
		positions.push_back(quadC_2.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(1);
		texCoords.push_back(1);

		positions.push_back(quadC_3.x);
		positions.push_back(quadC_3.y);
		positions.push_back(quadC_3.z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(1);


		unsigned int idx = 0;
		indices.push_back(idx + 0);
		indices.push_back(idx + 1);
		indices.push_back(idx + 2);
		indices.push_back(idx + 0);
		indices.push_back(idx + 2);
		indices.push_back(idx + 3);

		idx += 4;
		indices.push_back(idx + 0);
		indices.push_back(idx + 1);
		indices.push_back(idx + 2);
		indices.push_back(idx + 0);
		indices.push_back(idx + 2);
		indices.push_back(idx + 3);

		idx += 4;
		indices.push_back(idx + 0);
		indices.push_back(idx + 1);
		indices.push_back(idx + 2);
		indices.push_back(idx + 0);
		indices.push_back(idx + 2);
		indices.push_back(idx + 3);
	}
};

#endif
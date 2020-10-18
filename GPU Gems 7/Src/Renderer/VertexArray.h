#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>


class VertexArray
{
public:
	
	VertexArray();
	~VertexArray();

	void Bind() const;
	void Unbind() const;

	void SetNumInstances(uint32_t instances);

	void AddVertexBuffer(float const* const vertexBuffer, size_t bufferSizeBytes, uint32_t elementsPerVertex);
	void AddInstancedVertexBuffer(float const* const vertexBuffer, size_t bufferSizeBytes, uint32_t elementsPerVertex, uint32_t attribDivisor);
	void AddInstancedVertexBuffer(const std::vector<glm::mat4>& matrices);

	uint32_t GetID() const;

private:

	friend class Renderer;

	uint32_t instances;
	
	struct VertexBuffer
	{
		uint32_t buffer_ID;
	};

	uint32_t vertex_array_ID = 0;

	uint32_t vertex_buffer_index = 0;
	std::vector<VertexBuffer> vertex_buffers;
};

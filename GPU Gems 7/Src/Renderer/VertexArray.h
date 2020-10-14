#pragma once

#include <cstdint>
#include <vector>


class VertexArray
{
public:
	
	VertexArray();
	//if this is used we will draw using instancing
	VertexArray(uint32_t intances);
	~VertexArray();

	void Bind() const;
	void Unbind() const;

	void AddVertexBuffer(const std::vector<float>& vertexBuffer, uint32_t elementsPerVertex);
	void AddInstancedVertexBuffer(const std::vector<float>& data, uint32_t elementsPerVertex, uint32_t attribDivisor);

	uint32_t GetID() const;

private:

	friend class Renderer;

	uint32_t m_instances;
	
	struct VertexBuffer
	{
		uint32_t m_vertexBufferID;
		uint32_t m_elementsPerVertex;
	};

	uint32_t m_vertexArrayID = 0;

	uint32_t m_VertexBufferIndex = 0;
	std::vector<VertexBuffer> m_vertexBuffers;
};

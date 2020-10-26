#include "VertexArray.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <cassert>


VertexArray::VertexArray() 
	: num_instances(1)
{
	glGenVertexArrays(1, &vertex_array_ID);
}

VertexArray::~VertexArray()
{
	for (uint32_t& b : vertex_buffers)
	{
		glDeleteBuffers(1, &b);
	}

	glDeleteVertexArrays(1, &vertex_array_ID);
}

void VertexArray::SetNumInstances(uint32_t instances)
{
	num_instances = instances;
}

void VertexArray::AddVertexBuffer(float const * const vertexBuffer, size_t bufferSizeBytes, uint32_t elementsPerVertex)
{
	uint32_t buffer;

	glBindVertexArray(vertex_array_ID);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, &vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertex_buffer_index);
	glVertexAttribPointer(vertex_buffer_index, elementsPerVertex, GL_FLOAT, false, elementsPerVertex * sizeof(float), (void*)0);

	vertex_buffer_index++;
	vertex_buffers.push_back(buffer);
}

void VertexArray::AddInstancedVertexBuffer(const std::vector<glm::mat4>& matrices)
{

	glBindVertexArray(vertex_array_ID);

	uint32_t buffer;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_STATIC_DRAW);

	
	glEnableVertexAttribArray(vertex_buffer_index);
	glVertexAttribPointer(vertex_buffer_index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glVertexAttribDivisor(vertex_buffer_index, 1);
	vertex_buffer_index++;

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(vertex_buffer_index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glVertexAttribDivisor(vertex_buffer_index, 1);
	vertex_buffer_index++;

	glEnableVertexAttribArray(vertex_buffer_index);
	glVertexAttribPointer(vertex_buffer_index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glVertexAttribDivisor(vertex_buffer_index, 1);
	vertex_buffer_index++;

	glEnableVertexAttribArray(vertex_buffer_index);
	glVertexAttribPointer(vertex_buffer_index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	glVertexAttribDivisor(vertex_buffer_index, 1);
	vertex_buffer_index++;


	vertex_buffers.push_back(buffer);
}

void VertexArray::AddInstancedVertexBuffer(float const * const vertexBuffer, size_t bufferSizeBytes, uint32_t elementsPerVertex, uint32_t attribDivisor)
{
	uint32_t buffer;

	glBindVertexArray(vertex_array_ID);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, &vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertex_buffer_index);
	glVertexAttribPointer(vertex_buffer_index, elementsPerVertex, GL_FLOAT, false, elementsPerVertex * sizeof(float), (void*)0);
	glVertexAttribDivisor(vertex_buffer_index, attribDivisor);

	vertex_buffer_index++;
	vertex_buffers.push_back(buffer);
}


void VertexArray::Bind() const
{
	glBindVertexArray(vertex_array_ID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

uint32_t VertexArray::GetID() const
{
	return vertex_array_ID;
}

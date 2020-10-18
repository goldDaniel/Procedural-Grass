#include "VertexArray.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <cassert>


VertexArray::VertexArray() 
	: m_instances(0)
{
	glGenVertexArrays(1, &m_vertexArrayID);
}

VertexArray::~VertexArray()
{
	for (VertexBuffer& b : m_vertexBuffers)
	{
		glDeleteBuffers(1, &b.m_vertexBufferID);
	}

	glDeleteVertexArrays(1, &m_vertexArrayID);
}

void VertexArray::SetNumInstances(uint32_t instances)
{
	m_instances = instances;
}

void VertexArray::AddVertexBuffer(float const * const vertexBuffer, size_t bufferSizeBytes, uint32_t elementsPerVertex)
{
	VertexBuffer buffer;

	glBindVertexArray(m_vertexArrayID);

	glGenBuffers(1, &buffer.m_vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, &vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(m_VertexBufferIndex);
	glVertexAttribPointer(m_VertexBufferIndex, elementsPerVertex, GL_FLOAT, false, elementsPerVertex * sizeof(float), (void*)0);

	m_VertexBufferIndex++;
	m_vertexBuffers.push_back(buffer);
}

void VertexArray::AddInstancedVertexBuffer(const std::vector<glm::mat4>& matrices)
{

	glBindVertexArray(m_vertexArrayID);

	VertexBuffer buffer;

	glGenBuffers(1, &buffer.m_vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_STATIC_DRAW);

	
	glEnableVertexAttribArray(m_VertexBufferIndex);
	glVertexAttribPointer(m_VertexBufferIndex, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glVertexAttribDivisor(m_VertexBufferIndex, 1);
	m_VertexBufferIndex++;

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(m_VertexBufferIndex, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glVertexAttribDivisor(m_VertexBufferIndex, 1);
	m_VertexBufferIndex++;

	glEnableVertexAttribArray(m_VertexBufferIndex);
	glVertexAttribPointer(m_VertexBufferIndex, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glVertexAttribDivisor(m_VertexBufferIndex, 1);
	m_VertexBufferIndex++;

	glEnableVertexAttribArray(m_VertexBufferIndex);
	glVertexAttribPointer(m_VertexBufferIndex, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	glVertexAttribDivisor(m_VertexBufferIndex, 1);
	m_VertexBufferIndex++;


	m_vertexBuffers.push_back(buffer);
}

void VertexArray::AddInstancedVertexBuffer(float const * const vertexBuffer, size_t bufferSizeBytes, uint32_t elementsPerVertex, uint32_t attribDivisor)
{
	VertexBuffer buffer;

	glBindVertexArray(m_vertexArrayID);

	glGenBuffers(1, &buffer.m_vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, &vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(m_VertexBufferIndex);
	glVertexAttribPointer(m_VertexBufferIndex, elementsPerVertex, GL_FLOAT, false, elementsPerVertex * sizeof(float), (void*)0);
	glVertexAttribDivisor(m_VertexBufferIndex, attribDivisor);

	m_VertexBufferIndex++;
	m_vertexBuffers.push_back(buffer);
}


void VertexArray::Bind() const
{
	glBindVertexArray(m_vertexArrayID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

uint32_t VertexArray::GetID() const
{
	return m_vertexArrayID;
}

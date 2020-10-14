#include "VertexArray.h"

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
	buffer.m_elementsPerVertex = elementsPerVertex;

	glBindVertexArray(m_vertexArrayID);

	glGenBuffers(1, &buffer.m_vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, &vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(m_VertexBufferIndex);
	glVertexAttribPointer(m_VertexBufferIndex, elementsPerVertex, GL_FLOAT, false, elementsPerVertex * sizeof(float), (void*)0);

	m_VertexBufferIndex++;
	m_vertexBuffers.push_back(buffer);
}

void VertexArray::AddInstancedVertexBuffer(float const * const vertexBuffer, size_t bufferSizeBytes, uint32_t elementsPerVertex, uint32_t attribDivisor)
{
	VertexBuffer buffer;
	buffer.m_elementsPerVertex = elementsPerVertex;

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

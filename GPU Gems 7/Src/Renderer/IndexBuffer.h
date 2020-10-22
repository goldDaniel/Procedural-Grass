#ifndef INDEX_BUFFER_H_
#define INDEX_BUFFER_H_

#include <cstdint>
#include <vector>
#include <glad/glad.h>

class IndexBuffer
{
public:
	IndexBuffer(const std::vector<unsigned int>& indices)
	{
		glGenBuffers(1, &m_indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * indices.size(),
			&indices[0],
			GL_STATIC_DRAW);

		//assuming 32 bits will hold indices count
		m_numElements = static_cast<uint32_t>(indices.size());
	}

	~IndexBuffer()
	{
		glDeleteBuffers(1, &m_indexBufferID);
	}

	uint32_t GetID() const
	{
		return m_indexBufferID;
	}

	uint32_t GetNumElements() const
	{
		return m_numElements;
	}

private:
	
	friend class Renderer;

	


	uint32_t m_indexBufferID = 0;
	uint32_t m_numElements = 0;
};

#endif
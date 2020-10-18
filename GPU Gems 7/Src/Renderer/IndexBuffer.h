#pragma once

#include <cstdint>
#include <glad/glad.h>

class IndexBuffer
{
public:
	IndexBuffer(const std::vector<unsigned int>& indices)
	{
		glGenBuffers(1, &buffer_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_ID);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * indices.size(),
			&indices[0],
			GL_STATIC_DRAW);

		//assuming 32 bits will hold indices count
		num_elements = static_cast<uint32_t>(indices.size());
	}

	~IndexBuffer()
	{
		glDeleteBuffers(1, &buffer_ID);
	}

	uint32_t GetID() const
	{
		return buffer_ID;
	}

	uint32_t GetNumElements() const
	{
		return num_elements;
	}

private:
	
	friend class Renderer;

	uint32_t buffer_ID = 0;
	uint32_t num_elements = 0;
};

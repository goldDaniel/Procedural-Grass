#pragma once
#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include "Shader.h"
#include"VertexArray.h" 
#include "IndexBuffer.h"
#include "Texture.h"

#include <vector>

class Renderable
{
private:
	friend class Renderer;

	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 view = glm::mat4(1.0);
	glm::mat4 proj = glm::mat4(1.0);

	Shader const* const shader;
	VertexArray const* const vertex_array;
	IndexBuffer const* const index_buffer;

	std::vector<std::string> texture_names;
	std::vector<Texture2D*> textures;

	bool blending_enabled;

public:
	Renderable(const Shader* const shader,
		const VertexArray* const v,
		const IndexBuffer* const i);

	~Renderable();

	void Bind();

	void AddTexture(const std::string& uniform_name, Texture2D* const texture);

	void SetModelMatrix(const glm::mat4& model);
	void SetViewMatrix(const glm::mat4& view);
	void SetProjectionMatrix(const glm::mat4& proj);
};

#endif
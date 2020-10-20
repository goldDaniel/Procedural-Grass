#pragma once
#ifndef RENDERER_H_
#define RENDERER_H_

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#include "Texture.h"
#include "Renderable.h"

#include <unordered_map>


class Renderer
{
public:

	Renderer();
	~Renderer();

	void SetClearColor(const glm::vec4& color);
	void Clear();
	void RenderSkybox(glm::mat4 view, glm::mat4 proj);
	void Render(const Renderable& renderable) const;

	void Destroy(Renderable* renderable);

	void RenderWireframes(bool wireframes);

	VertexArray* const CreateVertexArray();
	IndexBuffer* const CreateIndexBuffer(const std::vector<unsigned int>& indices);
	Shader* const CreateShader(const char* vertexPath, const char* indexPath);
	Renderable* const CreateRenderable(Shader* shader, VertexArray* v, IndexBuffer* i);
	Texture2D* const CreateTexture2D(const std::string& filepath);


private:

	unsigned int loadCubemap(std::vector<std::string> faces);
	Renderable* CreateSkyboxRenderable();

	unsigned int skyboxCubemap;
	Renderable* skyboxRenderable;

	

	std::vector<std::unique_ptr<Renderable>> renderables;
	std::vector<std::unique_ptr<VertexArray>> vertexArrays;
	std::vector<std::unique_ptr<IndexBuffer>> indexBuffers;

	std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;

	std::unordered_map<std::string, std::unique_ptr<Texture2D>> textures;
};

#endif
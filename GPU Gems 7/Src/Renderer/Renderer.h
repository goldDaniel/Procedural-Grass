#pragma once
#ifndef RENDERER_H_
#define RENDERER_H_

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#include "Texture.h"
#include "Renderable.h"


class Renderer
{
public:

	Renderer();
	~Renderer();

	void SetClearColor(const glm::vec4& color);
	void Clear();
	void RenderSkybox(glm::mat4 view, glm::mat4 proj);
	void Render(const Renderable& renderable) const;

	void RenderInstanced(const Renderable& renderable, uint32_t numInstances) const;

	void RenderWireframes(bool wireframes);

	VertexArray* const CreateVertexArray();
	IndexBuffer* const CreateIndexBuffer(const std::vector<unsigned int>& indices);
	Shader* const CreateShader(const char* vertexPath, const char* indexPath);
	Renderable* const CreateRenderable(Shader* shader, VertexArray* v, IndexBuffer* i);
	Texture2D* const CreateTexture2D(const std::string& filepath);

	void ImGuiRender();

private:

	unsigned int loadCubemap(std::vector<std::string> faces);
	Renderable* CreateSkyboxRenderable();

	unsigned int skyboxCubemap;
	Renderable* skyboxRenderable;

	

	std::vector<Shader*> shaders;
	std::vector<VertexArray*> vertexArrays;
	std::vector<IndexBuffer*> indexBuffers;
};

#endif
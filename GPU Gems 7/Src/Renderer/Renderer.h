#ifndef RENDERER_H_
#define RENDERER_H_

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

class VertexArray;
class IndexBuffer;
class Renderable;
class Shader;
class Texture;

class Renderer
{
public:

	Renderer();
	~Renderer();

	void SetClearColor(const glm::vec4& color);
	void Clear();
	void UpdateViewport(int w, int h);

	void RenderSkybox(glm::mat4 view, glm::mat4 proj);
	void Render(const Renderable & renderable) const;
	void RenderWireframes(bool wireframes);


	VertexArray* const CreateVertexArray();
	IndexBuffer* const CreateIndexBuffer(const std::vector<unsigned int>& indices);
	Shader* const CreateShader(const char* vertexPath, const char* indexPath);
	Renderable* const CreateRenderable(Shader* shader, VertexArray* v, IndexBuffer* i);
	Texture* const CreateTexture2D(const std::string& filepath);

private:

	Renderable* CreateSkyboxRenderable();

	bool render_wireframes = false;

	unsigned int skybox_cubemap;
	Renderable* skybox_renderable;

	

	std::vector<std::unique_ptr<Renderable>> renderables;
	std::vector<std::unique_ptr<VertexArray>> vertex_arrays;
	std::vector<std::unique_ptr<IndexBuffer>> index_buffers;

	std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;

	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};

#endif
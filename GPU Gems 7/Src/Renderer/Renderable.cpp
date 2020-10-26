#include "Renderable.h"

Renderable::Renderable(const Shader* const shader,
	const VertexArray* const v,
	const IndexBuffer* const i)

	: shader(shader), vArray(v), iBuffer(i), render_backface(false)
{

}

Renderable::~Renderable() {}

void Renderable::AddTexture(std::string const& uniform_name, Texture2D* const texture)
{
	texture_names.push_back(uniform_name);
	textures.push_back(texture);
}

void Renderable::SetModelMatrix(const glm::mat4& model)
{
	this->model = model;
}

void Renderable::SetViewMatrix(const glm::mat4& view)
{
	this->view = view;
}

void Renderable::SetProjectionMatrix(const glm::mat4& proj)
{
	this->proj = proj;
}

void Renderable::Bind() const
{
	vArray->Bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer->GetID());
}

void Renderable::SetRenderBackface(bool enabled)
{
	render_backface = enabled;
}


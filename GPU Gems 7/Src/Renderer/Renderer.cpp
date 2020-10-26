#include "Renderer.h"

#include <SDL.h>

////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC
////////////////////////////////////////////////////////////////////////////////////////

Renderer::Renderer()
{
	skybox_renderable = CreateSkyboxRenderable();
	glClearColor(0, 0, 0, 1);

	glEnable(GL_BLEND);
}

Renderer::~Renderer()
{
	index_buffers.clear();
	vertex_arrays.clear();
	shaders.clear();
}

void Renderer::SetClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::UpdateViewport(int w, int h)
{
	glViewport(0, 0, w, h);
}

void Renderer::RenderSkybox(glm::mat4 view, glm::mat4 proj)
{
	glDepthFunc(GL_LEQUAL);
	skybox_renderable->shader->Bind();
	skybox_renderable->shader->SetMat4("projection", proj);
	skybox_renderable->shader->SetMat4("view", view);


	glBindVertexArray(skybox_renderable->vArray->GetID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_renderable->iBuffer->GetID());


	skybox_renderable->shader->SetInt("skybox", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap);

	glDrawElements(GL_TRIANGLES, skybox_renderable->iBuffer->GetNumElements(), GL_UNSIGNED_INT, 0);

	glDepthFunc(GL_LESS);
}

void Renderer::Render(const Renderable& renderable) const
{
	if (renderable.render_backface)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
	}
	
	renderable.shader->SetBool("render_wireframes", render_wireframes);

	//setup all our needed matrices
	renderable.shader->Bind();
	renderable.shader->SetMat4("model", renderable.model);
	renderable.shader->SetMat4("view", renderable.view);
	renderable.shader->SetMat4("projection", renderable.proj);

	//binds all textures in order they are added to the renderable
	for (uint32_t i = 0; i < renderable.textures.size(); i++)
	{
		renderable.shader->SetInt(renderable.texture_names[i], i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, renderable.textures[i]->GetID());
	}

	glBindVertexArray(renderable.vArray->GetID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.iBuffer->GetID());

	uint32_t instances = renderable.vArray->num_instances;
	glDrawElementsInstanced(GL_TRIANGLES, renderable.iBuffer->GetNumElements(), GL_UNSIGNED_INT, 0, instances);
}


void Renderer::RenderWireframes(bool wireframes)
{
	render_wireframes = wireframes;
	if (wireframes)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}


VertexArray* const Renderer::CreateVertexArray()
{
	auto resource = std::make_unique<VertexArray>();
	auto result = resource.get();

	vertex_arrays.push_back(std::move(resource));

	return result;
}

IndexBuffer* const Renderer::CreateIndexBuffer(const std::vector<unsigned int>& indices)
{
	auto resource = std::make_unique<IndexBuffer>(indices);
	auto result = resource.get();

	index_buffers.push_back(std::move(resource));

	return result;
}

Shader* const Renderer::CreateShader(const char* vertex_path, const char* frag_path)
{
	std::string key = vertex_path;
	key += frag_path;

	auto shaderIter = shaders.find(key);
	if (shaderIter != shaders.end())
	{
		return shaders[key].get();
	}

	shaders[key] = std::make_unique<Shader>(vertex_path, frag_path);

	return shaders[key].get();
}


Renderable* const Renderer::CreateRenderable(Shader* shader, VertexArray* v, IndexBuffer* i)
{
	for (auto& renderable : renderables)
	{
		if (renderable->shader == shader &&
			renderable->vArray == v &&
			renderable->iBuffer == i)
		{
			return renderable.get();
		}
	}

	auto resource = std::make_unique<Renderable>(shader, v, i);
	auto result = resource.get();

	renderables.push_back(std::move(resource));

	return result;
}

Texture2D* const Renderer::CreateTexture2D(const std::string& filepath)
{
	auto texturesIter = textures.find(filepath);
	if (texturesIter != textures.end())
	{
		return textures[filepath].get();
	}
	
	textures[filepath] = std::make_unique<Texture2D>(filepath);

	return textures[filepath].get();
}

////////////////////////////////////////////////////////////////////////////////////////
//PRIVATE
////////////////////////////////////////////////////////////////////////////////////////

unsigned int Renderer::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

Renderable* Renderer::CreateSkyboxRenderable()
{
	std::vector<std::string> cubemap_faces =
	{
		"Assets/Textures/Skybox/bluecloud_ft.jpg",
		"Assets/Textures/Skybox/bluecloud_bk.jpg",
								
		"Assets/Textures/Skybox/bluecloud_up.jpg",
		"Assets/Textures/Skybox/bluecloud_dn.jpg",
								
		"Assets/Textures/Skybox/bluecloud_rt.jpg",
		"Assets/Textures/Skybox/bluecloud_lf.jpg",
	};

	std::vector<float> skybox_vertices = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	std::vector<unsigned int> skybox_indices;
	for (unsigned int i = 0; i < 36; i++)
	{
		skybox_indices.push_back(i);
	}

	Shader* skybox_shader = CreateShader("Assets/Shaders/SkyboxVertex.glsl", "Assets/Shaders/SkyboxFragment.glsl");

	VertexArray* skybox_vArray = CreateVertexArray();
	skybox_vArray->AddVertexBuffer(&skybox_vertices[0], sizeof(float) * skybox_vertices.size(), 3);
	IndexBuffer* skybox_iBuffer = CreateIndexBuffer(skybox_indices);


	skybox_cubemap = loadCubemap(cubemap_faces);
	skybox_renderable = CreateRenderable(skybox_shader, skybox_vArray, skybox_iBuffer);
	
	return skybox_renderable;
}
#include "Renderer.h"

////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC
////////////////////////////////////////////////////////////////////////////////////////

Renderer::Renderer()
{
	skyboxRenderable = CreateSkyboxRenderable();
	glClearColor(0, 0, 0, 1);

	glEnable(GL_BLEND);
}

Renderer::~Renderer()
{
	for (IndexBuffer* i : indexBuffers)
	{
		delete i;
	}

	for (VertexArray* v : vertexArrays)
	{
		delete v;
	}

	for (Shader* s : shaders)
	{
		delete s;
	}
}

void Renderer::SetClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderSkybox(glm::mat4 view, glm::mat4 proj)
{
	glDepthFunc(GL_LEQUAL);
	skyboxRenderable->shader->Bind();
	skyboxRenderable->shader->SetMat4("projection", proj);
	skyboxRenderable->shader->SetMat4("view", view);


	glBindVertexArray(skyboxRenderable->vArray->GetID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxRenderable->iBuffer->GetID());


	skyboxRenderable->shader->SetInt("skybox", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap);

	glDrawElements(GL_TRIANGLES, skyboxRenderable->iBuffer->GetNumElements(), GL_UNSIGNED_INT, 0);

	glDepthFunc(GL_LESS);
}

void Renderer::Render(const Renderable& renderable) const
{
	renderable.shader->Bind();
	renderable.shader->SetMat4("model", renderable.model);
	renderable.shader->SetMat4("view", renderable.view);
	renderable.shader->SetMat4("projection", renderable.proj);


	for (uint32_t i = 0; i < renderable.textures.size(); i++)
	{
		renderable.shader->SetInt(renderable.textureNames[i], i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, renderable.textures[i]->GetID());
	}

	glBindVertexArray(renderable.vArray->GetID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.iBuffer->GetID());

	uint32_t instances = renderable.vArray->m_instances;
	if (instances > 0)
	{
		glDrawElementsInstanced(GL_TRIANGLES, renderable.iBuffer->GetNumElements(), GL_UNSIGNED_INT, 0, instances);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, renderable.iBuffer->GetNumElements(), GL_UNSIGNED_INT, 0);
	}
}


void Renderer::RenderWireframes(bool wireframes)
{
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
	auto resource = new VertexArray();
	vertexArrays.push_back(resource);

	return resource;
}

IndexBuffer* const Renderer::CreateIndexBuffer(const std::vector<unsigned int>& indices)
{
	auto resource = new IndexBuffer(indices);
	indexBuffers.push_back(resource);

	return resource;
}

Shader* const Renderer::CreateShader(const char* vertexPath, const char* indexPath)
{
	auto shader = new Shader(vertexPath, indexPath);
	shaders.push_back(shader);

	return shader;
}


Renderable* const Renderer::CreateRenderable(Shader* shader, VertexArray* v, IndexBuffer* i)
{
	return new Renderable(shader, v, i);
}

Texture2D* const Renderer::CreateTexture2D(const std::string& filepath)
{
	Texture2D* tex = Texture2D::CreateTexture(filepath);

	return tex;
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
		"Assets/Textures/Skybox/graycloud_ft.jpg",
		"Assets/Textures/Skybox/graycloud_bk.jpg",

		"Assets/Textures/Skybox/graycloud_up.jpg",
		"Assets/Textures/Skybox/graycloud_dn.jpg",

		"Assets/Textures/Skybox/graycloud_rt.jpg",
		"Assets/Textures/Skybox/graycloud_lf.jpg",
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


	skyboxCubemap = loadCubemap(cubemap_faces);
	skyboxRenderable = CreateRenderable(skybox_shader, skybox_vArray, skybox_iBuffer);
	
	return skyboxRenderable;
}
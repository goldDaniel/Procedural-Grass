#pragma once

#ifndef TEXTURE_H_
#define TEXTURE_H_


#include <cstdint>
#include <iostream>
#include <string>

#include <glad/glad.h>

#include "Vendor/stb_image/stb_image.h"

class Texture2D
{

public:

	static Texture2D * CreateTexture(const std::string& filepath)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		int width, height, nrChannels;
		
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			if(nrChannels == 3)
			{ 
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else if (nrChannels == 4)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Texture2D requires either 3 or 4 channels: " << nrChannels << " - at: "   << filepath << std::endl;
			}

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture2D failed to load at path: " << filepath << std::endl;
			stbi_image_free(data);
		}
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		return new Texture2D(textureID, width, height);
	}

	static void DestroyTexture(Texture2D* tex)
	{
		
	}

	uint32_t GetID() const
	{
		return ID;
	}

	int GetWidth() const
	{
		return width;
	}

	int GetHeight() const
	{
		return height;
	}

private:
	
	Texture2D(uint32_t ID, int w, int h) : ID(ID), width(w), height(h)
	{

	}

	uint32_t ID;

	int width;
	int height;
};


#endif

#pragma once

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <Core/Core.h>

class Texture
{
public:
	
	Texture(uint32_t ID, int w, int h);

	~Texture();

	const uint32_t ID;

	const int width;
	const int height;

	static Texture* Create(const std::string& filepath);
	static unsigned int LoadCubemap(const std::vector<std::string>& faces);	
};


#endif

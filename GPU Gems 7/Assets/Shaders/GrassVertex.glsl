
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec3 aNormal;


out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	Normal = aNormal;
	TexCoords = aTexCoord;
	gl_Position = projection * view * model * vec4((aPos + vec3(0,1,0)) * 4 + aOffset, 1.0f);
}


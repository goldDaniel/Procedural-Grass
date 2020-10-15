#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;


layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aTransform;

out vec3 Normal;
out vec2 TexCoords;

uniform float windOffset0;
uniform float windOffset1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
	Normal = aNormal;
	TexCoords = aTexCoord;


	vec4 posFinal = vec4(aPos, 1.0);
	
	if(aTexCoord.y > 0.5)
	{
		vec4 modified = aTransform * posFinal;

		posFinal.x += sin(windOffset0 + modified.x / 32.0);
		posFinal.z += cos(windOffset1 + modified.z / 16.0);
	}

	gl_Position = projection * view * aTransform * posFinal;
}









#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D grass;

uniform float time;

void main()
{
    vec3 lightDir = normalize(vec3(1,1,1));
    vec3 ambientLight = vec3(0.2, 0.2, 0.2);

    // ambient
    vec3 ambient = ambientLight * texture(grass, TexCoords.xy).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal.rgb);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.7, 0.7, 0.7) * texture(grass, TexCoords.xy).rgb * diff;
    
    vec3 result = ambient + diffuse;

    FragColor = vec4(result, 1.0);
}


#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D grass_bilboard;


void main()
{
    vec3 lightDir = normalize(vec3(1,1,1));
    vec3 ambientLight = vec3(0.2, 0.2, 0.2);

    // ambient
    vec3 ambient = ambientLight * texture(grass_bilboard, TexCoords.xy).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal.rgb);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.7, 0.7, 0.7) * texture(grass_bilboard, TexCoords.xy).rgb * diff;
    
    vec3 result = ambient + diffuse;

    FragColor = vec4(result, texture(grass_bilboard, TexCoords.xy).a);    
    if(FragColor.a < 0.1) discard;
}


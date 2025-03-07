#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D albedoMap;

void main()
{    
    vec3 color = texture(albedoMap, TexCoords).rgb;

    FragColor = vec4(color, 1.0);
}

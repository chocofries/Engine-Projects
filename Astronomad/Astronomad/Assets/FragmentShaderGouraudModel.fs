#version 450 core

in vec3 fragVertex;
in vec3 fragNormal;
in vec2 fragTexCoord;

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main(void)
{
	color = vec4(ambient + diffuse + specular, 1.0f) * (texture(texture_diffuse1, fragTexCoord));
}
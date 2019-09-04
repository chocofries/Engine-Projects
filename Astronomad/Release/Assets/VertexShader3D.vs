#version 450 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 fragVertex;
out vec3 fragNormal;
out vec2 fragTexCoord;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 transInvModel;

void main(void)
{
	gl_Position = MVP * vec4(vertex, 1.0);
	fragVertex = vec3(model * vec4(vertex, 1.0f));
	fragNormal = mat3(transInvModel)*normal;
	fragTexCoord = texCoord;
}
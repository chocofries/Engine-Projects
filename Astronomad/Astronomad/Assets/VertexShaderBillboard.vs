#version 450 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 fragColor;
out vec2 fragTexCoord;

uniform mat4 model;
uniform mat4 scale;
uniform mat4 view;
uniform mat4 proj;
uniform vec2 texScale;
uniform vec2 texOffset;

void main(void)
{
	mat4 mv = view * model;
	
	mv[0][0] = scale[0][0];
	mv[0][1] = 0.0f;
	mv[0][2] = 0.0f;
	
	mv[1][0] = 0.0f;
	mv[1][1] = scale[1][1];
	mv[1][2] = 0.0f;
	
	mv[2][0] = 0.0f;
	mv[2][1] = 0.0f;
	mv[2][2] = scale[2][2];
	
	vec4 pos = mv * vec4(vertex, 1.0);
	
	gl_Position = proj * pos;
	fragColor = color;
	fragTexCoord = (texCoord + texOffset) * texScale;
}
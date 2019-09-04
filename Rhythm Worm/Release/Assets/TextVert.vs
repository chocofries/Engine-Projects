#version 450 core
layout (location = 0) in vec4 vertex;

out vec2 fragTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform int isWorld;

void main()
{
	if (isWorld == 1)
	{
		gl_Position = proj * view * model * vec4(vertex.xyz, 1.0);
	}
	else
	{
		gl_Position = proj * vec4(vertex.xy, 0.0, 1.0);
	}
	fragTexCoord = vertex.zw;
}
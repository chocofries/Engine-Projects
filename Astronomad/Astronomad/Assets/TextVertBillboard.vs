#version 450 core
layout (location = 0) in vec4 vertex;

out vec2 fragTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 scale;
uniform int isWorld;

void main()
{
	if (isWorld == 1)
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
		
		vec4 pos = mv * vec4(vertex.xyz, 1.0);
	
		gl_Position = proj * pos;
	}
	else
	{
		gl_Position = proj * vec4(vertex.xy, 0.0, 1.0);
	}
	fragTexCoord = vertex.zw;
}
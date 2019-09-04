#version 450 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 fragVertex;
out vec3 fragNormal;
out vec2 fragTexCoord;

out vec3 ambient;
out vec3 diffuse;
out vec3 specular;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 transInvModel;

uniform float ambientStr;
uniform vec3 ambientColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 camPos;
uniform float lightSpecStr = 1.0f;
uniform float shininess = 16.0f;

void main(void)
{
	gl_Position = MVP * vec4(vertex, 1.0);
	fragVertex = vec3(model * vec4(vertex, 1.0f));
	fragNormal = mat3(transInvModel)*normal;
	fragTexCoord = texCoord;
	
	//------ Lighting calculations ------
	
	//Ambient
	ambient = ambientStr * ambientColor;
	
	//Light Direction
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(fragVertex - lightPos);
	
	//Diffuse
	float diffuseStr = max(dot(norm, -lightDir),0.0f);
	diffuse = diffuseStr * lightColor;
	
	//Specular
	vec3 negViewDir = normalize(camPos - fragVertex);
	vec3 reflectDir = reflect(lightDir, norm);
	float spec = pow(max(dot(negViewDir, reflectDir), 0.0f), shininess);
	specular = lightSpecStr * spec * lightColor;
}
#version 450 core

in vec3 fragVertex;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform float ambientStr;
uniform vec3 ambientColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 camPos;
uniform float lightSpecStr = 1.0f;
uniform float shininess = 16.0f;

uniform int rimExponent = 4;
uniform vec3 rimColor = vec3(1.0f, 1.0f, 1.0f);

uniform samplerCube skybox;
uniform float reflectStr = 0.2f;

void main(void)
{
	//Ambient
	vec3 ambient = ambientStr * ambientColor;
	
	//Light Direction
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(fragVertex - lightPos);
	
	//Diffuse
	float diffuseStr = max(dot(norm, -lightDir),0.0f);
	vec3 diffuse = diffuseStr * lightColor;
	
	//Specular
	vec3 negViewDir = normalize(camPos - fragVertex);
	vec3 halfwayVec = normalize(-lightDir + negViewDir);
	float spec = pow(max(dot(norm, halfwayVec), 0.0f), shininess);
	vec3 specular = lightSpecStr * spec * lightColor;
	
	//Rim
	float rimFactor = 1.0f - dot(norm, negViewDir);
	rimFactor = smoothstep(0.0f, 1.0f, rimFactor);
	rimFactor = pow(rimFactor, rimExponent);
	vec3 rim = rimFactor * rimColor * lightColor;
	
	//Reflection
	vec3 viewDir = normalize(fragVertex - camPos);
	vec3 reflectDir = reflect(viewDir, norm);
	
	color = vec4(mix((ambient + diffuse + specular + rim),(texture(skybox, reflectDir).rgb*4.0f),reflectStr), 1.0f) * (texture(texture_diffuse1, fragTexCoord));
}
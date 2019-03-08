#version 330 core
out vec4 FragColor;
struct Material{
	vec3 ambient; //ambient strength
	vec3 diffuse; //
	vec3 specular;//specular strength
	float shininess;
};
struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;
uniform Material material;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

//in vec2 TexCoords;
//uniform sampler2D ourtexture;


void main( )
{
	//ambient
	float ambientStrength=0.5;
	vec3 ambient=light.ambient*material.ambient;
	//diffuse
	vec3 norm=normalize(Normal);
	vec3 lightDir=normalize(lightPos-FragPos);
	float diff=max(dot(norm,lightDir),0.0f);
	vec3 diffuse=light.diffuse*(diff*material.diffuse);

	//specular
	float specularStrength=0.5;
	vec3 viewDir=normalize(viewPos-FragPos);
	vec3 reflectDir=reflect(-lightDir,norm);
	float spec=pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular=light.specular*material.specular*spec;

	vec3 result=(ambient+diffuse+specular);
	FragColor=vec4(result,1.0f);
   
    //color = vec4( texture( ourtexture, TexCoords ));
   //FragColor=vec4(0.0f,0.0f,1.0f,1.0f);
}
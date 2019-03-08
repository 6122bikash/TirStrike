#version 330 core

out vec4 FragColor;
struct Material{
	
	sampler2D diffuse; //
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

in vec2 TexCoords;
//uniform sampler2D ourtexture;


void main( )
{
	//ambient
	float ambientStrength=0.5;
	vec3 ambient=light.ambient*vec3(texture(material.diffuse,TexCoords));
	//diffuse
	vec3 norm=normalize(Normal);
	vec3 lightDir=normalize(lightPos-FragPos);
	float diff=max(dot(norm,lightDir),0.0f);
	vec3 diffuse=light.diffuse*(diff*vec3(texture(material.diffuse,TexCoords)));

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
}/*
#version 330 core

in vec2 TexCoords;

out vec4 color;
vec3 guncolor=vec3(1.0f,0.5f,0.31f);
vec3 lightcolor=vec3(1.0f,1.0f,1.0f);
uniform sampler2D ourtexture;
//uniform sampler2D texture_diffuse;

void main( )
{
    color = vec4( texture( ourtexture, TexCoords ));
   //color=vec4(guncolor*lightcolor,1.0f);
}
*/
#version 330 core
out vec4 FragColor;
in vec2 texcoord;
in vec3 FragPos;
in vec3 Normal;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
vec3 norm=normalize(Normal);
vec3 lightDir=normalize(vec3(0.5f,1.0f,-10.0f)-FragPos);
vec3 lightColor=vec3(1.0f,1.0f,1.0f);
float diff=max(dot(norm,lightDir),0.0);
vec3 diffuse=diff*lightColor;
vec3 objectColor=vec3(0.0f,1.0f,0.0f);
float ambientStrength=0.1f;
vec3 ambient=ambientStrength*lightColor;
vec3 result=(ambient+diffuse)*objectColor;
	//FragColor=mix(texture(texture1,texcoord),texture(texture2,texcoord),0.8);
	FragColor=vec4(result,1.0);

}
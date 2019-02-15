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
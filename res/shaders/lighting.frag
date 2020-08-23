#version 330 core
out vec4 color;

uniform vec3 objectColor; //Contain colour of the object itself
uniform vec3 lightColor;

void main()
{
    color = vec4(lightColor * objectColor, 1.0f);
}


//Fragment - colour and texture
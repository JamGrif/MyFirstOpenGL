#version 330 core
out vec4 color;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor; //Contain colour of the object itself
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

//Phong shading

//Ambient Lighting
//General light that is always bouncing around the room, no particular origin, position or direction
//Not light from the sun

//Diffuse
//Takes into account direction and normal of the light

//Specular
//Little shiny light shining off an object

void main()
{
    //Ambient Light
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    //Diffuse Light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); //Tell you the direction the light is pointing
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor; 

    //Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result,1.0f);


}


//Fragment - colour and texture
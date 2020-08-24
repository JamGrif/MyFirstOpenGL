#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 color;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

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
    //Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));

    //Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos); //Tell you the direction the light is pointing
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); 

    //Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    color = vec4(ambient + diffuse + specular, 1.0f);

}


//Fragment - colour and texture
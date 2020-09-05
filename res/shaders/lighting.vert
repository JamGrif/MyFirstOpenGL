#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model; //Converts local-object coordinates to camera coordinates
uniform mat4 view; //Converts normalized coordinates to window ccoordinates
uniform mat4 projection; //Converts camera coordinates to normalized coordinates

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;
}


//Vertex - location and positioning


//Phong shading
//Ambient Lighting
//General light that is always bouncing around the room, no particular origin, position or direction
//Not light from the sun

//Diffuse
//Takes into account direction and normal of the light

//Specular
//Little shiny light shining off an object+

//Attenuation
//Attenuation lighting is a decrease in lighting as you move away from an object
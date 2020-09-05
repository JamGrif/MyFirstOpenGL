#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model; //Converts local-object coordinates to camera coordinates
uniform mat4 view; //Converts normalized coordinates to window ccoordinates
uniform mat4 projection; //Converts camera coordinates to normalized coordinates

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
   
}


//Vertex - location and positioning
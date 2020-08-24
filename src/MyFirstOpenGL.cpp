#include <iostream>

//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

//SOIL
#include "SOIL2.h"

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Other includes
#include "Shader.h"
#include "Camera.h"

//Window Dimensions
const GLint Width = 800;
const GLint Height = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

//Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = Width / 2.0f;
GLfloat lastY = Height / 2.0f;

bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 cubePos(0.0f, 0.0f, 0.0f);
//Position of the source of light in world
glm::vec3 lightPos(2.0f, 0.0f, -0.5f);

int main()
{

	//Init GLFW
	glfwInit();

	//Set required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Required for MacOS
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //Stop resize of window

	glEnable(GL_BLEND); //Help in blending image in the shape
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Enabled alpha support

	//Create a GLFW window that can use GLFW functions
	GLFWwindow* window = glfwCreateWindow(Width, Height, "Jamie Griffiths", nullptr, nullptr);

	GLFWimage icons[1];
	icons[0].pixels = SOIL_load_image("res/textures/cowboy.jpg", &icons[0].width, &icons[0].height, 0, SOIL_LOAD_RGBA);
	glfwSetWindowIcon(window, 1, icons);
	SOIL_free_image_data(icons[0].pixels);

	//Gets actual size of screen, relative to density of screen
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	//Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	//Remove mouse cursor 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//GLEW use a modern approach allowing retrieve function pointers and extensions
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit())
	{
		std::cout << "Failure to initialize GLEW" << std::endl;

		return EXIT_FAILURE;
	}

	//Setup OpenGL viewport
	//Define viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//Build and compile the shader programs
	Shader lightingShader("res/shaders/lighting.vs", "res/shaders/lighting.frag");
	Shader lampShader("res/shaders/lamp.vs", "res/shaders/lamp.frag");

	// Set up our vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};

	GLuint VBO; //Vertex Buffer Object
	GLuint containerVAO; //Vertex Array Object

	glGenVertexArrays(1, &containerVAO);
	glGenBuffers(1, &VBO);

	//Bind vertex array first
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(containerVAO);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);




	//Set lights VAO
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	//Only need to bind VBO as it contains all data we need
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Set vertex attributes (only position data for the lamp)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//Load textures
	GLuint diffuseMap, specularMap;
	glGenTextures(1, &diffuseMap);
	glGenTextures(1, &specularMap);

	int textureWidth, textureHeight;
	unsigned char* image;

	//Diffuse Map
	image = SOIL_load_image("res/textures/container2.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST_MIPMAP_NEAREST);

	//Diffuse Map
	image = SOIL_load_image("res/textures/container2_specular.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	//Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	//Using perspective projection
	glm::mat4 projection = glm::mat4(1.0f);
	//FOV, Aspect ratio, near clipping plane, far clipping plane
	projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	float x = 0;
	float x2 = 0;
	bool MoveForward = true;
	bool MoveForward2 = false;
	
	//Game loop
	while (!glfwWindowShouldClose(window))
	{
		//Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Check if any events have been activiated
		glfwPollEvents();
		DoMovement();

		//Clear the colourbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear out window, ready to draw next frame

		lightingShader.Use();
        GLint lightPosLoc = glGetUniformLocation( lightingShader.Program, "light.position" );
        GLint viewPosLoc = glGetUniformLocation( lightingShader.Program, "viewPos" );

		//Uniforming the objects colour location
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		//Set lights properties
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);

		//Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		//Create transformations
		glm::mat4 view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		/*
		if (MoveForward2)
		{
			x2 += 1.0f * deltaTime;
		}
		else
		{
			x2 -= 1.0f * deltaTime;
		}

		if (x2 >= 2) { MoveForward2 = false; }
		if (x2 <= -2) { MoveForward2 = true; }

		cubePos.z = x2;
		cubePos.y = x2 * 2;
		*/

		//Get their uniform location
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		//Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//Bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		//Bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		//Draw the container using its vertex attributes
		glBindVertexArray(containerVAO);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePos);
		//model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//--------------------
		/*
		if (MoveForward) 
		{
			x += 1.5f * deltaTime;
		}
		else 
		{
			x -= 1.5f * deltaTime;
		}

		if (x >= 2) { MoveForward = false; }
		if (x <= -2) {MoveForward = true; }

		lightPos.z = x;
		*/
		//std::cout << lightPos.z << std::endl;
		//std::cout << lightPos.y << std::endl;
		//Draw lamp object, bind the appropriate shader
		lampShader.Use();

		//Get their uniform location
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		//Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); //Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//Swap the screen buffers
		glfwSwapBuffers(window);

	}

	//De-allocate all resources 
	glDeleteVertexArrays(1, &containerVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	//Terminate GLFW
	glfwTerminate();

	return EXIT_SUCCESS;
}

//Moves the camera positions based on user input
void DoMovement()
{
	//Camera controls
	if (keys[GLFW_KEY_W])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S] )
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A] )
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (keys[GLFW_KEY_RIGHT]) 
	{
		lightPos.x += 1 * deltaTime;
	}
	if (keys[GLFW_KEY_LEFT])
	{
		lightPos.x -= 1 * deltaTime;
	}
	if (keys[GLFW_KEY_UP])
	{
		lightPos.z -= 1 * deltaTime;
	}
	if (keys[GLFW_KEY_DOWN])
	{
		lightPos.z += 1 * deltaTime;
	}
}

//Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos; //Reversed as Y coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}
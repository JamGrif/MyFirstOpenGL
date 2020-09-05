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
//#include "Texture.h"
#include "Model.h"

//Window Dimensions
const GLint Width = 1920;
const GLint Height = 1080;
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
glm::vec3 lightPos(1.2f, 1.0f, -2.0f);

int main()
{

	//Init GLFW
	glfwInit();

	//Set required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

	//Setup shaders
	Shader shader("res/shaders/cube.vert", "res/shaders/cube.frag");

	//Load the model
	Model ourModel("res/models/nanosuit.obj");

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

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

		shader.Use();

		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		// Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); 
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		ourModel.Draw(shader);

		// Swap the screen buffers
		glfwSwapBuffers(window);

	}

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
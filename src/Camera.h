#pragma once

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//Default camera values
const GLfloat YAW		 = -90.0f;
const GLfloat PITCH		 = 0.0f;
const GLfloat SPEED		 = 6.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM		 = 45.0f;

class Camera
{
public:
    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateCameraVectors();
    }

    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM)
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateCameraVectors();
    }

    //Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() 
    {
        //Stating where we want to look (front) 
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }
	

    //Process input from keyboard
    //Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->movementSpeed * deltaTime;

        //Multiple if statements to allow multiple keys pressed down
        if (direction == FORWARD)
        {
            this->position += this->front * velocity;
        }

        if (direction == BACKWARD)
        {
            this->position -= this->front * velocity;
        }

        if (direction == LEFT)
        {
            this->position -= this->right * velocity;
        }

        if (direction == RIGHT)
        {
            this->position += this->right * velocity;
        }

    }

    //Process input from mouse. Expects mouse offset value in both x and y direction
    void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true) 
    {
        xOffset *= this->mouseSensitivity;
        yOffset *= this->mouseSensitivity;

        this->yaw += xOffset;
        this->pitch += yOffset;

        //Make sure that when pitch is out of bounds, screen doesnt get flipped
        if (constrainPitch) 
        {
            if (this->pitch > 89.0f) 
            {
                this->pitch = 89.0;
            }
            if (this->pitch < -89.0f) 
            {
                this->pitch = -89.0f;
            }
        }

        //Update Front, Right and Up vectors using the updated eular angles
        this->updateCameraVectors();

    }

    //Process mouse scroll (vertical scroll only)
    void ProcessMouseScroll(GLfloat yOffset) 
    {
        if (this->zoom >= 1.0f && this->zoom <= 45.0f)
        {
            this->zoom -= yOffset;
        }

        if (this->zoom <= 1.0f)
        {
            this->zoom = 1.0f;
        }

        if (this->zoom >= 45.0f)
        {
            this->zoom = 45.0f;
        }
    }

    GLfloat GetZoom() 
    {
        return this->zoom;
    }

    glm::vec3 GetPosition()
    {
        return this->position;
    }

    glm::vec3 GetFront()
    {
        return this->front;
    }

private:
    //Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    //Eular Angles
    GLfloat yaw;
    GLfloat pitch;

    //Camera Options
    GLfloat movementSpeed;
    GLfloat mouseSensitivity;
    GLfloat zoom;

    // Calculates the front vector from the Camera's updated Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        this->front = glm::normalize(front);

        // Also re-calculate the Right and Up vector
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }



};



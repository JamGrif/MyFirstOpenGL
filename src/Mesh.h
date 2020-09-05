#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;
};

struct Texture
{
	GLuint id;
	std::string type;
	aiString path;
};


class Mesh 
{
public:
	//Mesh data
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		//Set up the mesh
		this->SetupMesh();
	}

	//Render the mesh
	void Draw(Shader shader)
	{
		//Bind appropriate textures
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;

		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			//Active proper texture before binding
			glActiveTexture(GL_TEXTURE0 + i);

			//Retrieve texture number
			std::stringstream ss;
			std::string number;
			std::string name = this->textures[i].type;

			if (name == "texture_diffuse")
			{
				ss << diffuseNr++; //Transfer GLuint to stream
			}
			else if (name == "texture_specular")
			{
				ss << specularNr++; //Transfer GLuint to stream
			}

			number = ss.str();

			//Set sampler to correct texture unit
			glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);

			//Bind texture
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}

		//Set mesh's shininess property to a default value
		glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);

		//Draw mesh
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//Reset back to default values
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

private:

	GLuint VAO, VBO, EBO;

	void SetupMesh()
	{
		//Create buffers
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);
		glBindVertexArray(this->VAO);

		//Load data into vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		//Vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

		//Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));


		//Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		//Unbind the vertex array
		glBindVertexArray(0);


	}


};

#include "../include/IndexedCubeRenderable.hpp"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "../include/Utils.hpp"
#include "../include/gl_helper.hpp"
#include "./../include/log.hpp"

IndexedCubeRenderable::IndexedCubeRenderable(ShaderProgramPtr shaderProgram)
    : Renderable(shaderProgram), m_vBuffer(0), m_cBuffer(0), m_iBuffer(0)
{
	// Positions des sommets du cube
	m_positions.push_back(glm::vec3(-0.5, -0.5, -0.5));
	m_positions.push_back(glm::vec3(0.5, -0.5, -0.5));
	m_positions.push_back(glm::vec3(0.5, 0.5, -0.5));
	m_positions.push_back(glm::vec3(-0.5, 0.5, -0.5));
	m_positions.push_back(glm::vec3(-0.5, -0.5, 0.5));
	m_positions.push_back(glm::vec3(-0.5, 0.5, 0.5));
	m_positions.push_back(glm::vec3(0.5, 0.5, 0.5));
	m_positions.push_back(glm::vec3(0.5, -0.5, 0.5));

	// Couleurs pour chaque face du cube (12 triangles, 3 sommets par triangle) (merci Copilot)
	m_colors.push_back(glm::vec4(0, 0, 0, 1));  // Noir
	m_colors.push_back(glm::vec4(1, 0, 0, 1));  // Rouge
	m_colors.push_back(glm::vec4(1, 1, 0, 1));  // Jaune
	m_colors.push_back(glm::vec4(0, 1, 0, 1));  // Vert
	m_colors.push_back(glm::vec4(0, 0, 1, 1));  // Bleu
	m_colors.push_back(glm::vec4(0, 1, 1, 1));  // Cyan
	m_colors.push_back(glm::vec4(1, 1, 1, 1));  // Blanc
	m_colors.push_back(glm::vec4(1, 0, 1, 1));  // Magenta

	// Indexes des sommets pour chaque triangle du cube
	m_indices.push_back(glm::uvec3(0, 2, 1));
	m_indices.push_back(glm::uvec3(1, 6, 7));
	m_indices.push_back(glm::uvec3(1, 2, 6));
	m_indices.push_back(glm::uvec3(5, 4, 7));
	m_indices.push_back(glm::uvec3(5, 7, 6));
	m_indices.push_back(glm::uvec3(4, 3, 0));
	m_indices.push_back(glm::uvec3(4, 5, 3));
	m_indices.push_back(glm::uvec3(3, 6, 2));
	m_indices.push_back(glm::uvec3(0, 3, 2));
	m_indices.push_back(glm::uvec3(3, 5, 6));
	m_indices.push_back(glm::uvec3(4, 0, 1));
	m_indices.push_back(glm::uvec3(4, 1, 7));

	// Set the model matrix to identity
	m_model = glm::mat4(1.0);

	// Create buffers
	glGenBuffers(1, &m_vBuffer);  // vertices
	glGenBuffers(1, &m_cBuffer);  // vertices
	glGenBuffers(1, &m_iBuffer);  // indexes

	// Activate buffer and send data to the graphics card
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_colors.size() * sizeof(glm::vec4), m_colors.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(glm::uvec3), m_indices.data(), GL_STATIC_DRAW);
}

void IndexedCubeRenderable::do_draw()
{
	// Get the identifier ( location ) of the uniform modelMat in the shader program
	int modelLocation = m_shaderProgram->getUniformLocation("modelMat");
	// Send the data corresponding to this identifier on the GPU
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(m_model));

	// Get the identifier of the attribute vPosition in the shader program
	int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
	// Activate the attribute array at this location
	glEnableVertexAttribArray(positionLocation);
	// Bind the position buffer on the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	// Specify the location and the format of the vertex position attribute
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	int colorLocation = m_shaderProgram->getAttributeLocation("vColor");
	glEnableVertexAttribArray(colorLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer);
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// For index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer);
	// Draw openGL primitives
	glDrawElements(GL_TRIANGLES, m_indices.size() * 3, GL_UNSIGNED_INT, (void*)0);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, m_positions.size());

	// Release the vertex attribute array
	glDisableVertexAttribArray(positionLocation);
	glDisableVertexAttribArray(colorLocation);
}

IndexedCubeRenderable::~IndexedCubeRenderable()
{
	glcheck(glDeleteBuffers(1, &m_vBuffer));
	glcheck(glDeleteBuffers(1, &m_cBuffer));
	glcheck(glDeleteBuffers(1, &m_iBuffer));
}

#include "../include/CubeRenderable.hpp"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "../include/Utils.hpp"
#include "../include/gl_helper.hpp"
#include "./../include/log.hpp"

CubeRenderable::CubeRenderable(ShaderProgramPtr shaderProgram)
	: Renderable(shaderProgram), m_vBuffer(0), m_cBuffer(0)
{
	// provide an lvalue for normals so a non-const reference can be bound
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	getUnitCube(m_positions, normals, uvs);

	// Couleurs pour chaque face du cube (12 triangles, 3 sommets par triangle) (merci Copilot)
	m_colors.push_back(glm::vec4(1, 0, 0, 1)); // Rouge
	m_colors.push_back(glm::vec4(1, 0, 0, 1));
	m_colors.push_back(glm::vec4(1, 0, 0, 1));

	m_colors.push_back(glm::vec4(0, 1, 0, 1)); // Vert
	m_colors.push_back(glm::vec4(0, 1, 0, 1));
	m_colors.push_back(glm::vec4(0, 1, 0, 1));

	m_colors.push_back(glm::vec4(0, 0, 1, 1)); // Bleu
	m_colors.push_back(glm::vec4(0, 0, 1, 1));
	m_colors.push_back(glm::vec4(0, 0, 1, 1));

	m_colors.push_back(glm::vec4(1, 1, 0, 1)); // Jaune
	m_colors.push_back(glm::vec4(1, 1, 0, 1));
	m_colors.push_back(glm::vec4(1, 1, 0, 1));

	m_colors.push_back(glm::vec4(1, 0, 1, 1)); // Magenta
	m_colors.push_back(glm::vec4(1, 0, 1, 1));
	m_colors.push_back(glm::vec4(1, 0, 1, 1));

	m_colors.push_back(glm::vec4(0, 1, 1, 1)); // Cyan
	m_colors.push_back(glm::vec4(0, 1, 1, 1));
	m_colors.push_back(glm::vec4(0, 1, 1, 1));

	m_colors.push_back(glm::vec4(1, 0.5, 0, 1)); // Orange
	m_colors.push_back(glm::vec4(1, 0.5, 0, 1));
	m_colors.push_back(glm::vec4(1, 0.5, 0, 1));

	m_colors.push_back(glm::vec4(0.5, 0, 1, 1)); // Violet
	m_colors.push_back(glm::vec4(0.5, 0, 1, 1));
	m_colors.push_back(glm::vec4(0.5, 0, 1, 1));

	m_colors.push_back(glm::vec4(0, 0.5, 1, 1)); // Bleu clair
	m_colors.push_back(glm::vec4(0, 0.5, 1, 1));
	m_colors.push_back(glm::vec4(0, 0.5, 1, 1));

	m_colors.push_back(glm::vec4(0.5, 1, 0, 1)); // Vert clair
	m_colors.push_back(glm::vec4(0.5, 1, 0, 1));
	m_colors.push_back(glm::vec4(0.5, 1, 0, 1));

	m_colors.push_back(glm::vec4(1, 0, 0.5, 1)); // Rose
	m_colors.push_back(glm::vec4(1, 0, 0.5, 1));
	m_colors.push_back(glm::vec4(1, 0, 0.5, 1));

	m_colors.push_back(glm::vec4(0.5, 1, 1, 1)); // Turquoise
	m_colors.push_back(glm::vec4(0.5, 1, 1, 1));
	m_colors.push_back(glm::vec4(0.5, 1, 1, 1));

	// model matrix is provided by the base Renderable (initialized to identity)

	// Create buffers
	glGenBuffers(1, &m_vBuffer);  // vertices
	glGenBuffers(1, &m_cBuffer);  // vertices

	// Activate buffer and send data to the graphics card
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_colors.size() * sizeof(glm::vec4), m_colors.data(), GL_STATIC_DRAW);
}

void CubeRenderable::do_draw()
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
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, m_positions.size());

	// Release the vertex attribute array
	glDisableVertexAttribArray(positionLocation);
}

CubeRenderable::~CubeRenderable()
{
	glcheck(glDeleteBuffers(1, &m_vBuffer));
	glcheck(glDeleteBuffers(1, &m_cBuffer));
}

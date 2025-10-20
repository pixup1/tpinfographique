#include "./../include/CylinderMeshRenderable.hpp"

#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/Utils.hpp"
#include "../include/gl_helper.hpp"
#include "./../include/log.hpp"

CylinderMeshRenderable::~CylinderMeshRenderable()
{
}

CylinderMeshRenderable::CylinderMeshRenderable(ShaderProgramPtr shaderProgram, bool indexed, unsigned int slices, bool vertex_normals) : MeshRenderable(shaderProgram, indexed)
{
	if (indexed)
	{
		std::vector<glm::uvec3> indices;
		getUnitIndexedCylinder(m_positions, m_normals, indices, 3);
		// getUniIndexedCube fills a std::vector<glm::uvec3> of length n,
		// but m_indices is a std::vector<unsigned int> of length 3n.
		// We need to unpack the values.
		unpack(indices, m_indices);

		// Set random colors per vertex
		m_colors.resize(m_positions.size(), glm::vec4(0));
		for (size_t i = 0; i < m_colors.size(); ++i)
			m_colors[i] = randomColor();
	}
	else
	{
		// Go to Utils.cpp and fill this function
		getUnitCylinder(m_positions, m_normals, m_tcoords, slices, vertex_normals);
		// Set random colors per triangle
		m_colors.resize(m_positions.size(), glm::vec4(0));
		for (size_t i = 0; i < m_colors.size() / 12; ++i)
		{
			glm::vec4 topColor = glm::vec4(1, 0, 0, 1);
			glm::vec4 botColor = glm::vec4(0, 1, 0, 1);
			glm::vec4 latColor = glm::vec4(0, 0, 1, 1);

			m_colors[12 * i + 0] = topColor;
			m_colors[12 * i + 1] = topColor;
			m_colors[12 * i + 2] = topColor;

			m_colors[12 * i + 3] = latColor;
			m_colors[12 * i + 4] = latColor;
			m_colors[12 * i + 5] = latColor;
			m_colors[12 * i + 6] = latColor;
			m_colors[12 * i + 7] = latColor;
			m_colors[12 * i + 8] = latColor;

			m_colors[12 * i + 9] = botColor;
			m_colors[12 * i + 10] = botColor;
			m_colors[12 * i + 11] = botColor;
		}
	}

	// See MeshRenderable::update_all_buffers
	update_all_buffers();
}
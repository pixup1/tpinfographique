#ifndef CUBE_RENDERABLE_HPP
#define CUBE_RENDERABLE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "Renderable.hpp"

class CubeRenderable : public Renderable
{
   public:
	~CubeRenderable();
	CubeRenderable(ShaderProgramPtr program);

   private:
	void do_draw();

	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec4> m_colors;
	std::vector<glm::vec3> m_indexes;
	unsigned int m_vBuffer;
	unsigned int m_cBuffer;
	unsigned int m_iBuffer;

	// Use m_model from base class Renderable (do not shadow it here)
};

typedef std::shared_ptr<CubeRenderable> CubeRenderablePtr;

#endif

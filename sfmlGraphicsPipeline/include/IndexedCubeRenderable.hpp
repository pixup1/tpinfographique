#ifndef INDEXED_CUBE_RENDERABLE_HPP
#define INDEXED_CUBE_RENDERABLE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "Renderable.hpp"

class IndexedCubeRenderable : public Renderable
{
   public:
	~IndexedCubeRenderable();
	IndexedCubeRenderable(ShaderProgramPtr program);

   private:
	void do_draw();

	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec4> m_colors;
	std::vector<glm::uvec3> m_indices;
	unsigned int m_vBuffer;
	unsigned int m_cBuffer;
	unsigned int m_iBuffer;

	// Use m_model from base class Renderable (do not shadow it here)
};

typedef std::shared_ptr<IndexedCubeRenderable> IndexedCubeRenderablePtr;

#endif

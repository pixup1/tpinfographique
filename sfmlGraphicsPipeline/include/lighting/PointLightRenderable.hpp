#ifndef POINT_LIGHT_RENDERABLE_HPP
#define POINT_LIGHT_RENDERABLE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "../MeshRenderable.hpp"
#include "Light.hpp"

class PointLightRenderable : public MeshRenderable
{
   public:
	~PointLightRenderable();
	PointLightRenderable(const ShaderProgramPtr& program, const PointLightPtr& light, unsigned int strips = 10u, unsigned int slices = 20u);

   private:
	void do_animate(float time);

	PointLightPtr m_light;
};

typedef std::shared_ptr<PointLightRenderable> PointLightRenderablePtr;

#endif

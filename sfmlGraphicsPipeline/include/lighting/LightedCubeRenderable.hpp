#ifndef LIGHTED_CUBE_RENDERABLE_HPP
#define LIGHTED_CUBE_RENDERABLE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "LightedMeshRenderable.hpp"
#include "Material.hpp"

class LightedCubeRenderable : public LightedMeshRenderable
{
   public:
	~LightedCubeRenderable();
	LightedCubeRenderable(ShaderProgramPtr program, bool indexed, const MaterialPtr& material);
};

typedef std::shared_ptr<LightedCubeRenderable> LightedCubeRenderablePtr;

#endif

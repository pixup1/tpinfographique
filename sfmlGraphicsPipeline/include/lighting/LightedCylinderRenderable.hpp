#ifndef LIGHTED_CYLINDER_RENDERABLE_HPP
#define LIGHTED_CYLINDER_RENDERABLE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "LightedMeshRenderable.hpp"
#include "Material.hpp"

class LightedCylinderRenderable : public LightedMeshRenderable
{
   public:
	~LightedCylinderRenderable();
	LightedCylinderRenderable(ShaderProgramPtr prog, bool indexed, const MaterialPtr& mat, unsigned int slice = 30u, bool vertex_normals = true);
};

typedef std::shared_ptr<LightedCylinderRenderable> LightedCylinderRenderablePtr;

#endif

#ifndef LIGHTED_CYLINDER_RENDERABLE_HPP
#define LIGHTED_CYLINDER_RENDERABLE_HPP

#include "LightedMeshRenderable.hpp"
#include "Material.hpp"
#include <vector>
#include <glm/glm.hpp>

class LightedCylinderRenderable : public LightedMeshRenderable
{
public:
    ~LightedCylinderRenderable();
    LightedCylinderRenderable(ShaderProgramPtr prog, bool indexed, const MaterialPtr & mat, unsigned int slice=30u, bool vertex_normals=true);
};

typedef std::shared_ptr<LightedCylinderRenderable> LightedCylinderRenderablePtr;

#endif

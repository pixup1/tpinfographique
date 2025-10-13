#ifndef TEXTURED_PLANE_RENDERABLE_HPP
#define TEXTURED_PLANE_RENDERABLE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "TexturedMeshRenderable.hpp"

class TexturedPlaneRenderable : public TexturedMeshRenderable
{
   public:
	~TexturedPlaneRenderable();
	TexturedPlaneRenderable(ShaderProgramPtr shaderProgram, const std::string& filename);
};

typedef std::shared_ptr<TexturedPlaneRenderable> TexturedPlaneRenderablePtr;

#endif

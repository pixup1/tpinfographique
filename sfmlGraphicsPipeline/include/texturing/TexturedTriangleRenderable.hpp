#ifndef TEXTURED_TRIANGLE_RENDERABLE_HPP
#define TEXTURED_TRIANGLE_RENDERABLE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "TexturedMeshRenderable.hpp"

class TexturedTriangleRenderable : public TexturedMeshRenderable
{
   public:
	~TexturedTriangleRenderable();
	TexturedTriangleRenderable(ShaderProgramPtr shaderProgram, const std::string& filename);
};

typedef std::shared_ptr<TexturedTriangleRenderable> TexturedTriangleRenderablePtr;

#endif

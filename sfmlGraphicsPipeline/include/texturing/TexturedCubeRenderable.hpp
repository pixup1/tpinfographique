#ifndef TEXTURED_CUBE_RENDERABLE_HPP
#define TEXTURED_CUBE_RENDERABLE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "TexturedMeshRenderable.hpp"

class TexturedCubeRenderable : public TexturedMeshRenderable
{
   public:
	~TexturedCubeRenderable();
	TexturedCubeRenderable(ShaderProgramPtr shaderProgram, const std::string& filename);
};

typedef std::shared_ptr<TexturedCubeRenderable> TexturedCubeRenderablePtr;

#endif

#include "../include/KeyframedHierarchicalRenderable.hpp"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "../include/GeometricTransformation.hpp"
#include "../include/Utils.hpp"
#include "../include/gl_helper.hpp"

KeyframedHierarchicalRenderable::KeyframedHierarchicalRenderable(ShaderProgramPtr prog)
    : HierarchicalRenderable(prog)
{
}

void KeyframedHierarchicalRenderable::addLocalTransformKeyframe(const GeometricTransformation& transformation, float time)
{
	m_localKeyframes.add(transformation, time);
}

void KeyframedHierarchicalRenderable::addGlobalTransformKeyframe(const GeometricTransformation& transformation, float time)
{
	m_globalKeyframes.add(transformation, time);
}

void KeyframedHierarchicalRenderable::do_animate(float time)
{
	// Assign the interpolated transformations from the keyframes to the local/global transformations.
	if (!m_localKeyframes.empty())
	{
		setLocalTransform(m_localKeyframes.interpolateTransformation(time));
	}
	if (!m_globalKeyframes.empty())
	{
		setGlobalTransform(m_globalKeyframes.interpolateTransformation(time));
	}
}

KeyframedHierarchicalRenderable::~KeyframedHierarchicalRenderable()
{
}

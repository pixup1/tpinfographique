#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <GeometricTransformation.hpp>
#include <ShaderProgram.hpp>
#include <Viewer.hpp>
#include <iomanip>
#include <iostream>

void movingTree(Viewer& viewer);
void movingCylinder(Viewer& viewer);

void initialize_scene(Viewer& viewer)
{
	movingCylinder(viewer);
	// movingTree(viewer);
}

int main()
{
	glm::vec4 background_color(0.7, 0.7, 0.7, 1);
	Viewer viewer(1280, 720, background_color);
	initialize_scene(viewer);

	while (viewer.isRunning())
	{
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();
	}

	return EXIT_SUCCESS;
}

void movingCylinder(Viewer& viewer)
{
	// Add shader
	ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
	                                                              "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
	viewer.addShaderProgram(flatShader);

	// Frame
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	// Animated cylinder
	auto cylinder = std::make_shared<CylinderMeshRenderable>(flatShader, false, 20, false);
	cylinder->setGlobalTransform(glm::mat4(1.0));

	cylinder->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, 0.0f),
	                                                             glm::quat(),
	                                                             glm::vec3(1.0f, 1.0f, 1.0f)),
	                                     0.0f);
	cylinder->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, 0.0f),
	                                                             glm::angleAxis(1.0f, glm::vec3(0.0f, 1.0f, 0.0f)),
	                                                             glm::vec3(1.0f, 1.0f, 1.0f)),
	                                     1.0f);

	cylinder->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, -5.0f),
	                                                             glm::quat(),
	                                                             glm::vec3(1.0f, 1.0f, 1.0f)),
	                                     0.0f);
	cylinder->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, -5.0f),
	                                                             glm::angleAxis(2.0f, glm::vec3(0.0f, 0.0f, 1.0f)),
	                                                             glm::vec3(1.0f, 1.0f, 1.0f)),
	                                     1.0f);

	viewer.addRenderable(cylinder);
	viewer.startAnimation();
}

void movingTree(Viewer& viewer)
{
	// Add shader
	ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
	                                                              "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
	viewer.addShaderProgram(flatShader);

	// Frame
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	// Tree modeling:
	// The modeling is hierarchical (setLocalTransform, setGlobalTransform)
	// The animation is hierarchical too (addGlobalTransformKeyframe, addLocalTransformKeyframe)

	// TODO: Create and animate the main branch
	CylinderMeshRenderablePtr root = std::make_shared<CylinderMeshRenderable>(flatShader, false);
	root->setLocalTransform(GeometricTransformation(glm::vec3{}, glm::quat(), glm::vec3{0.1, 0.1, 2.0}).toMatrix());
	// TODO: root->addGlobalTransformKeyframe(...)
	// ...

	// TODO: Add and animate a child branch
	CylinderMeshRenderablePtr r1 = std::make_shared<CylinderMeshRenderable>(flatShader, false);
	// r1->addGlobalTransformKeyframe(...);
	// r1->addLocalTransformKeyframe(...);
	//  ...
	// HierarchicalRenderable::addChild( root, r1 );
	//  ...

	// TODO: Add and animate any other child branchs you want

	viewer.addRenderable(root);

	viewer.startAnimation();
}
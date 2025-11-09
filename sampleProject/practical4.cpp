#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <GeometricTransformation.hpp>
#include <ShaderProgram.hpp>
#include <Viewer.hpp>
#include <iomanip>
#include <iostream>

void movingTree(Viewer &viewer);
void movingCylinder(Viewer &viewer);

void initialize_scene(Viewer &viewer)
{
	//movingCylinder(viewer);
	movingTree(viewer);
	viewer.setAnimationLoop(true, 5.0);
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

void movingCylinder(Viewer &viewer)
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
																 glm::angleAxis(M_PIf * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f)),
																 glm::vec3(1.0f, 1.0f, 1.0f)),
										 5.0f);

	cylinder->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, -5.0f),
																glm::quat(),
																glm::vec3(1.0f, 1.0f, 1.0f)),
										0.0f);
	cylinder->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, -5.0f),
																glm::angleAxis(M_PIf * 1.5f, glm::vec3(0.0f, 0.0f, 1.0f)),
																glm::vec3(1.0f, 1.0f, 1.0f)),
										5.0f);

	viewer.addRenderable(cylinder);
	viewer.startAnimation();
}

void creer_branche(int level, HierarchicalRenderablePtr parent, ShaderProgramPtr shaderProgram, float lengthMultiplier, float angle, int branching_branches, float thickness)
{
	if (level == 0)
	{
		return;
	}
	for (int i = 0; i < branching_branches; ++i)
	{
		// Create child
		auto child = std::make_shared<CylinderMeshRenderable>(shaderProgram, false, 20u, false);
		// Set global transform
		child->setGlobalTransform(glm::mat4(1.0));
		
		glm::mat4 childGlobalTransform;
		childGlobalTransform *= getTranslationMatrix(0.0, 1.0, 0.0);
		childGlobalTransform *= getRotationMatrix(glm::radians(angle + i * (360.0f / branching_branches)), glm::vec3(0.0, 1.0, 0.0));
		childGlobalTransform *= getRotationMatrix(glm::radians(angle), glm::vec3(1.0, 0.0, 0.0));
		childGlobalTransform *= getScaleMatrix(lengthMultiplier, lengthMultiplier, lengthMultiplier);

		childGlobalTransform *= getRotationMatrix(glm::radians(10.0f), glm::vec3(1.0, 0.0, 0.0));
		child->addGlobalTransformKeyframe(childGlobalTransform, 0.0f);
		child->addGlobalTransformKeyframe(childGlobalTransform, 5.0f);
		
		childGlobalTransform *= getRotationMatrix(glm::radians(-10.0f), glm::vec3(1.0, 0.0, 0.0));
		child->addGlobalTransformKeyframe(childGlobalTransform, 2.5f);

		// Set local transform
		glm::mat4 childLocalTransform;
		childLocalTransform *= getScaleMatrix(thickness, 1.0, thickness);
		childLocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		child->setLocalTransform(childLocalTransform);
		// Define parent / children relationships
		HierarchicalRenderable::addChild(parent, child);
		// Recursive call
		creer_branche(level - 1, child, shaderProgram, lengthMultiplier, angle, branching_branches, thickness);
	}
}

void movingTree(Viewer &viewer)
{
	// Add shader
	ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
																  "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
	viewer.addShaderProgram(flatShader);

	// Frame
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	std::shared_ptr<CylinderMeshRenderable> root = std ::
		make_shared<CylinderMeshRenderable>(flatShader, false, 20u, false);
	
	glm::mat4 rootLocalTransform;
	rootLocalTransform *= getScaleMatrix(0.15, 1.0, 0.15);
	rootLocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	root->setLocalTransform(rootLocalTransform);

	creer_branche(5, root, flatShader, 0.75f, 35.0f, 4, 0.15);

	viewer.addRenderable(root);

	viewer.startAnimation();
}
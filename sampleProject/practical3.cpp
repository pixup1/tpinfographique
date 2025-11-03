#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <HierarchicalRenderable.hpp>
#include <MeshRenderable.hpp>
#include <ShaderProgram.hpp>
#include <SphereMeshRenderable.hpp>
#include <Viewer.hpp>

void creer_branche(int level, HierarchicalRenderablePtr parent, ShaderProgramPtr shaderProgram, float lengthMultiplier, float angle, int branching_branches, float thickness)
{
	if (level == 0)
	{
		return;
	}
	for (int i = 0; i < branching_branches; ++i)
	{
		// Create child
		std::shared_ptr<CylinderMeshRenderable> child = std::
		    make_shared<CylinderMeshRenderable>(shaderProgram, false, 20u, false);
		// Set global transform
		glm::mat4 childGlobalTransform;
		childGlobalTransform *= getTranslationMatrix(0.0, 1.0, 0.0);
		childGlobalTransform *= getRotationMatrix(glm::radians(angle + i * (360.0f / branching_branches)), glm::vec3(0.0, 1.0, 0.0));
		childGlobalTransform *= getRotationMatrix(glm::radians(angle), glm::vec3(1.0, 0.0, 0.0));
		childGlobalTransform *= getScaleMatrix(lengthMultiplier, lengthMultiplier, lengthMultiplier);
		child->setGlobalTransform(childGlobalTransform);
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

void initialize_scene(Viewer& viewer)
{
	ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(
	    "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
	    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");

	viewer.addShaderProgram(flatShader);

	// Frame
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	// Create renderables
	std::shared_ptr<CylinderMeshRenderable> root = std ::
	    make_shared<CylinderMeshRenderable>(flatShader, false, 20u, false);
	// For each element of the hierarchy ,
	// Set local transform and global transform
	glm::mat4 rootGlobalTransform;
	root->setGlobalTransform(rootGlobalTransform);
	glm::mat4 rootLocalTransform;
	rootLocalTransform *= getScaleMatrix(0.15, 1.0, 0.15);
	rootLocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	root->setLocalTransform(rootLocalTransform);
	// Define parent / children relationships
	creer_branche(5, root, flatShader, 0.75f, 35.0f, 4, 0.15);
	// Add the root of the hierarchy to the viewer
	viewer.addRenderable(root);
}

int main()
{
	Viewer viewer(1280, 720);
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

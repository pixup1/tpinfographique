#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <HierarchicalRenderable.hpp>
#include <MeshRenderable.hpp>
#include <ShaderProgram.hpp>
#include <SphereMeshRenderable.hpp>
#include <Viewer.hpp>

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
	std::shared_ptr<CylinderMeshRenderable> child1 = std ::
		make_shared<CylinderMeshRenderable>(flatShader, false, 20u, false);
	// For each element of the hierarchy ,
	// Set local transform and global transform
	glm::mat4 rootGlobalTransform;
	rootGlobalTransform *= getRotationMatrix(glm::radians(15.0f), glm::vec3(1.0, 0.0, 0.0));
	root -> setGlobalTransform(rootGlobalTransform);
	glm::mat4 rootLocalTransform;
	rootLocalTransform *= getScaleMatrix(0.2, 1.0, 0.2);
	rootLocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	root -> setLocalTransform(rootLocalTransform);
	glm::mat4 child1GlobalTransform;
	child1GlobalTransform *= getTranslationMatrix(0.0, 1.0, 0.0);
	child1GlobalTransform *= getRotationMatrix(glm::radians(15.0f), glm::vec3(0.0, 0.0, 1.0));
	child1 -> setGlobalTransform(child1GlobalTransform);
	glm::mat4 child1LocalTransform;
	child1LocalTransform *= getScaleMatrix(0.2, 1.0, 0.2);
	child1LocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	child1 -> setLocalTransform(child1LocalTransform);
	// Define parent / children relationships
	HierarchicalRenderable::addChild(root, child1);
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

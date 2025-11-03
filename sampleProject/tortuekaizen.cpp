#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <MeshRenderable.hpp>
#include <ShaderProgram.hpp>
#include <Viewer.hpp>

void initialize_scene(Viewer& viewer)
{
	// Create a shader program
	ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(
	    "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
	    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");

	// Add the shader program to the viewer
	viewer.addShaderProgram(flatShader);

	const std::string shell_path = "../ObjFiles/Carapace";
	const std::string nag_ard_path = "../ObjFiles/Nag-ArD";
	const std::string nag_arg_path = "../ObjFiles/Nag-ArG";
	const std::string nag_avd_path = "../ObjFiles/Nag-AvD";
	const std::string nag_avg_path = "../ObjFiles/Nag-AvG";
	const std::string tete_path = "../ObjFiles/Tete";
	MeshRenderablePtr shell = std::make_shared<MeshRenderable>(flatShader, shell_path + ".obj");
	MeshRenderablePtr nag_ard = std::make_shared<MeshRenderable>(flatShader, nag_ard_path + ".obj");
	MeshRenderablePtr nag_arg = std::make_shared<MeshRenderable>(flatShader, nag_arg_path + ".obj");
	MeshRenderablePtr nag_avd = std::make_shared<MeshRenderable>(flatShader, nag_avd_path + ".obj");
	MeshRenderablePtr nag_avg = std::make_shared<MeshRenderable>(flatShader, nag_avg_path + ".obj");
	MeshRenderablePtr tete = std::make_shared<MeshRenderable>(flatShader, tete_path + ".obj");
	HierarchicalRenderable::addChild(shell, nag_ard, true);
	HierarchicalRenderable::addChild(shell, nag_arg, true);
	HierarchicalRenderable::addChild(shell, nag_avd, true);
	HierarchicalRenderable::addChild(shell, nag_avg, true);
	HierarchicalRenderable::addChild(shell, tete, true);
	nag_ard->addKeyframesFromFile(nag_ard_path + ".animation", 0.0, false);
	nag_arg->addKeyframesFromFile(nag_arg_path + ".animation", 0.0, false);
	nag_avd->addKeyframesFromFile(nag_avd_path + ".animation", 0.0, false);
	nag_avg->addKeyframesFromFile(nag_avg_path + ".animation", 0.0, false);
	viewer.addRenderable(shell);
	viewer.startAnimation();
}

int main()
{
	glm::vec4 background_color(0.8, 0.8, 0.8, 1);
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
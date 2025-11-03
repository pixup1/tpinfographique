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

	const std::string shell_path = "../ObjFiles/Carapace.obj";
	const std::string nag_ard_path = "../ObjFiles/Nag-ArD.obj";
	const std::string nag_arg_path = "../ObjFiles/Nag-ArG.obj";
	const std::string nag_avd_path = "../ObjFiles/Nag-AvD.obj";
	const std::string nag_avg_path = "../ObjFiles/Nag-AvG.obj";
	const std::string tete_path = "../ObjFiles/Tete.obj";
	MeshRenderablePtr shell = std::make_shared<MeshRenderable>(flatShader, shell_path);
	MeshRenderablePtr nag_ard = std::make_shared<MeshRenderable>(flatShader, nag_ard_path);
	MeshRenderablePtr nag_arg = std::make_shared<MeshRenderable>(flatShader, nag_arg_path);
	MeshRenderablePtr nag_avd = std::make_shared<MeshRenderable>(flatShader, nag_avd_path);
	MeshRenderablePtr nag_avg = std::make_shared<MeshRenderable>(flatShader, nag_avg_path);
	MeshRenderablePtr tete = std::make_shared<MeshRenderable>(flatShader, tete_path);
	HierarchicalRenderable::addChild(shell, nag_ard);
	HierarchicalRenderable::addChild(shell, nag_arg);
	HierarchicalRenderable::addChild(shell, nag_avd);
	HierarchicalRenderable::addChild(shell, nag_avg);
	HierarchicalRenderable::addChild(shell, tete);
	viewer.addRenderable(shell);
	viewer.addRenderable(nag_ard);
	viewer.addRenderable(nag_arg);
	viewer.addRenderable(nag_avd);
	viewer.addRenderable(nag_avg);
	viewer.addRenderable(tete);
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
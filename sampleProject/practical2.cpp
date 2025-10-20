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

	// Create a cylinder
	bool indexed = false;         // indexed version already implemented
	unsigned int slices = 20u;    // number of slices
	bool vertex_normals = false;  // use vertex normals ? else triangle normals
	// See CylinderMeshRenderable.cpp
	CylinderMeshRenderablePtr cylinder = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);

	// Add the cylinder to the viewer
	//viewer.addRenderable(cylinder);

	// Create suzanne
	const std::string suzanne_path = "../../sfmlGraphicsPipeline/meshes/suzanne.obj";
	MeshRenderablePtr suzanne = std::make_shared<MeshRenderable>(flatShader, suzanne_path);
	suzanne->setModelMatrix(getTranslationMatrix(5, 0, 0));
	// Add suzanne to the viewer
	//viewer.addRenderable(suzanne);

	const std::string cat_path = "../../sfmlGraphicsPipeline/meshes/cat.obj";
	const std::string pillar_path = "../../sfmlGraphicsPipeline/meshes/pillar.obj";
	MeshRenderablePtr cat = std::make_shared<MeshRenderable>(flatShader, cat_path);
	MeshRenderablePtr pillar = std::make_shared<MeshRenderable>(flatShader, pillar_path);
	cat->setModelMatrix(getTranslationMatrix(0.5,3.2,0));
	pillar->setModelMatrix(getRotationMatrix(glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0)));
	viewer.addRenderable(cat);
	viewer.addRenderable(pillar);
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
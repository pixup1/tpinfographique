#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <MeshRenderable.hpp>
#include <lighting/LightedMeshRenderable.hpp>
#include <ShaderProgram.hpp>
#include <Viewer.hpp>
#include <lighting/PointLightRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <fstream>
#include <iostream>

LightedMeshRenderablePtr add_object(Viewer &viewer,
									const std::string &name,
									const MaterialPtr &material,
									ShaderProgramPtr &shaderProgram,
									HierarchicalRenderablePtr parent = nullptr)
{
	std::string obj_path = "../ObjFiles/" + name + ".obj";
	std::ifstream file(obj_path);
	if (!file.good())
	{
		std::cerr << "Error: File " << obj_path << " does not exist." << std::endl;
		return nullptr;
	}
	LightedMeshRenderablePtr obj = std::make_shared<LightedMeshRenderable>(shaderProgram, obj_path, material);
	if (parent != nullptr)
	{
		HierarchicalRenderable::addChild(parent, obj, true);
	}
	std::string anim_path = "../Animation/" + name + ".animation";
	std::ifstream anim_file(anim_path);
	if (anim_file.good())
	{
		obj->addKeyframesFromFile(anim_path, 0.0, false);
	}
	viewer.addRenderable(obj);

	return obj;
}

void initialize_scene(Viewer &viewer)
{
	// Shaders
	ShaderProgramPtr cartoonShader = std::make_shared<ShaderProgram>(
		"../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
		"../../sfmlGraphicsPipeline/shaders/cartoonFragment.glsl");
	ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
																	 "../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
	viewer.addShaderProgram(cubeMapShader);
	viewer.addShaderProgram(cartoonShader);
	
	// Materials
	MaterialPtr pearl = Material::Pearl();
	
	// Cubemap
	std::string cubemap_dir = "../Textures/skybox";
	auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
	viewer.addRenderable(cubemap);
	
	// Objects
	LightedMeshRenderablePtr ground = add_object(viewer, "Ground", pearl, cartoonShader);
	LightedMeshRenderablePtr ocean = add_object(viewer, "Ocean", pearl, cartoonShader);
	LightedMeshRenderablePtr palmiers = add_object(viewer, "Palmiers", pearl, cartoonShader);
	LightedMeshRenderablePtr leaves = add_object(viewer, "Leaves", pearl, cartoonShader);

	LightedMeshRenderablePtr shell = add_object(viewer, "Carapace", pearl, cartoonShader);
	LightedMeshRenderablePtr nag_ard = add_object(viewer, "Nag-ArD", pearl, cartoonShader, shell);
	LightedMeshRenderablePtr nag_arg = add_object(viewer, "Nag-ArG", pearl, cartoonShader, shell);
	LightedMeshRenderablePtr nag_avd = add_object(viewer, "Nag-AvD", pearl, cartoonShader, shell);
	LightedMeshRenderablePtr nag_avg = add_object(viewer, "Nag-AvG", pearl, cartoonShader, shell);
	LightedMeshRenderablePtr tete = add_object(viewer, "Tete", pearl, cartoonShader, shell);

	// Lights
	// Sun
	glm::vec3 d_direction = glm::normalize(glm::vec3(-1.0, -0.2, 1.0));
	glm::vec3 d_ambient(0.4, 0.4, 0.6), d_diffuse(1.5, 1.5, 1.5), d_specular(1.5, 1.5, 1.5);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	viewer.addDirectionalLight(directionalLight);

	// Define a point light
	// float p_constant = 1.0, p_linear = 5e-1, p_quadratic = 0.0;
	// glm::vec3 p_position = glm::vec3(2, 5.0, 5.0);
	// glm::vec3 p_ambient = glm::vec3(0.0, 0.0, 0.2);
	// glm::vec3 p_diffuse = glm::vec3(3.0, 3.0, 3.0);
	// glm::vec3 p_specular = glm::vec3(3.0, 3.0, 3.0);
	// PointLightPtr pointLight1 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse, p_specular, p_constant, p_linear, p_quadratic);
	// viewer.addPointLight(pointLight1);

	// Animate camera
	viewer.getCamera().addKeyframesFromFile("../Animation/Camera.animation", 0.0, false);
	
	// const std::string test_obj_path = "../ObjFiles/Tete.obj";
	// LightedMeshRenderablePtr test = std::make_shared<LightedMeshRenderable>(cartoonShader, test_obj_path, pearl);
	// test->addKeyframesFromFile("../Animation/Camera.animation", 0.0, false);
	// viewer.addRenderable(test);

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
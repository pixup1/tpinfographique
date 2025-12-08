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
#include <texturing/TexturedLightedMeshRenderable.hpp>

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
	auto obj = std::make_shared<LightedMeshRenderable>(shaderProgram, obj_path, material);
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

TexturedLightedMeshRenderablePtr add_textured_object(Viewer &viewer,
													 const std::string &name,
													 const MaterialPtr &material,
													 const std::string &texture_path,
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
	auto obj = std::make_shared<TexturedLightedMeshRenderable>(shaderProgram, obj_path, material, texture_path);
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
	ShaderProgramPtr cartoonTextureShader = std::make_shared<ShaderProgram>(
		"../../sfmlGraphicsPipeline/shaders/textureVertex.glsl",
		"../../sfmlGraphicsPipeline/shaders/cartoonTextureFragment.glsl");
	ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
																	 "../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
	viewer.addShaderProgram(cubeMapShader);
	viewer.addShaderProgram(cartoonTextureShader);
	viewer.addShaderProgram(cartoonShader);

	// Materials
	MaterialPtr nolighting = Material::NoLighting();
	MaterialPtr white = Material::White();
	MaterialPtr water = Material::Water();
	MaterialPtr bark = Material::Bark();
	MaterialPtr sand = Material::Sand();
	MaterialPtr pureblack = Material::PureBlack();
	MaterialPtr black = Material::Black();
	MaterialPtr orange = Material::BrightOrange();
	MaterialPtr rock = Material::Rock();

	// Cubemap
	std::string cubemap_dir = "../Textures/skybox";
	auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
	viewer.addRenderable(cubemap);

	// Objects
	auto titre = add_object(viewer, "Titre", orange, cartoonShader);
	auto titre_blackdrop = add_object(viewer, "TitreBlackdrop", pureblack, cartoonShader);
	
	auto ground = add_object(viewer, "Ground", sand, cartoonShader);
	auto ground_coral = add_textured_object(viewer, "GroundCoral", nolighting, "../Textures/Corail.png", cartoonTextureShader);
	auto ground_rocks = add_object(viewer, "GroundRocks", rock, cartoonShader);
	auto ocean = add_object(viewer, "Ocean", water, cartoonShader);
	auto palmiers = add_object(viewer, "Palmiers", bark, cartoonShader);
	auto leaves = add_textured_object(viewer, "Leaves", white, "../Textures/Feuille.png", cartoonTextureShader);

	auto house2 = add_object(viewer, "maison.001", white, cartoonShader);
	auto clock = add_textured_object(viewer, "Clock", nolighting, "../Textures/clock.jpg", cartoonTextureShader);
	auto hour_hand = add_object(viewer, "Hours", white, cartoonShader, clock);
	auto minute_hand = add_object(viewer, "Minutes", white, cartoonShader, clock);
	auto bed_frame = add_object(viewer, "BedFrame", bark, cartoonShader);
	auto bed_sheets = add_object(viewer, "BedSheets", white, cartoonShader);

	auto shell = add_textured_object(viewer, "Carapace.001", white, "../Textures/Tortue_bleue.png", cartoonTextureShader);
	auto nag_ard = add_textured_object(viewer, "Nag-ArD.001", white, "../Textures/Tortue_bleue.png", cartoonTextureShader, shell);
	auto nag_arg = add_textured_object(viewer, "Nag-ArG.001", white, "../Textures/Tortue_bleue.png", cartoonTextureShader, shell);
	auto nag_avd = add_textured_object(viewer, "Nag-AvD.001", white, "../Textures/Tortue_bleue.png", cartoonTextureShader, shell);
	auto nag_avg = add_textured_object(viewer, "Nag-AvG.001", white, "../Textures/Tortue_bleue.png", cartoonTextureShader, shell);
	auto tete = add_textured_object(viewer, "Tete.001", white, "../Textures/Tortue_bleue.png", cartoonTextureShader, shell);

	auto shell2 = add_textured_object(viewer, "Carapace.002", white, "../Textures/Tortue_orange.png", cartoonTextureShader);
	auto nag_ard2 = add_textured_object(viewer, "Nag-ArD.002", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell2);
	auto nag_arg2 = add_textured_object(viewer, "Nag-ArG.002", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell2);
	auto nag_avd2 = add_textured_object(viewer, "Nag-AvD.002", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell2);
	auto nag_avg2 = add_textured_object(viewer, "Nag-AvG.002", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell2);
	auto tete2 = add_textured_object(viewer, "Tete.002", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell2);

	auto tear = add_object(viewer, "Larme", water, cartoonShader);

	// Lights
	// Sun
	glm::vec3 d_direction = glm::normalize(glm::vec3(-1.0, -0.2, 1.0));
	glm::vec3 d_ambient(0.4, 0.4, 0.6), d_diffuse(1.5, 1.5, 1.5), d_specular(1.5, 1.5, 1.5);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	viewer.addDirectionalLight(directionalLight);

	float p_constant = 1.0, p_linear = 5e-1, p_quadratic = 0.0;
	glm::vec3 p_position = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 p_ambient = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 p_diffuse = glm::vec3(3.0, 3.0, 3.0);
	glm::vec3 p_specular = glm::vec3(3.0, 3.0, 3.0);
	PointLightPtr title_light1 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse * 0.9f, p_specular * 0.9f, p_constant, p_linear, p_quadratic);
	title_light1->applyObjTransform("../ObjFiles/TitreLight1.obj");
	PointLightPtr title_light2 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse * 0.7f, p_specular * 0.7f, p_constant, p_linear, p_quadratic);
	title_light2->addKeyframesFromFile("../Animation/TitreLight2.animation", 0.0, false);
	viewer.addPointLight(title_light1);
	viewer.addPointLight(title_light2);

	// Camera
	viewer.getCamera().setFov(0.5);
	viewer.getCamera().addKeyframesFromFile("../Animation/Camera.animation", 0.0, false);

	// Soundtrack
	viewer.setSoundtrack("../tortuekaizen.wav");

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
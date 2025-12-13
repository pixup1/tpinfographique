#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <MeshRenderable.hpp>
#include <ShaderProgram.hpp>
#include <Viewer.hpp>
#include <dynamics/ConstantForceField.hpp>
#include <dynamics/DampingForceField.hpp>
#include <dynamics/DynamicSystem.hpp>
#include <dynamics/DynamicSystemRenderable.hpp>
#include <dynamics/EulerExplicitSolver.hpp>
#include <dynamics/MushroomForceField.hpp>
#include <dynamics/ParticleListRenderable.hpp>
#include <dynamics/RadialImpulseForceField.hpp>
#include <fstream>
#include <glm/gtc/random.hpp>
#include <iostream>
#include <lighting/DirectionalLightRenderable.hpp>
#include <lighting/LightedMeshRenderable.hpp>
#include <lighting/PointLightRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <texturing/TexturedLightedMeshRenderable.hpp>

LightedMeshRenderablePtr add_object(Viewer& viewer,
                                    const std::string& name,
                                    const MaterialPtr& material,
                                    ShaderProgramPtr& shaderProgram,
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

TexturedLightedMeshRenderablePtr add_textured_object(Viewer& viewer,
                                                     const std::string& name,
                                                     const MaterialPtr& material,
                                                     const std::string& texture_path,
                                                     ShaderProgramPtr& shaderProgram,
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

void initialize_scene(Viewer &viewer, RadialImpulseForceFieldPtr &explosion, MushroomForceFieldPtr &mushroom, PointLightPtr &explosion_light, LightedMeshRenderablePtr &filter)
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
	ShaderProgramPtr particleShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/partVertex.glsl",
	                                                                  "../../sfmlGraphicsPipeline/shaders/partFragment.glsl");
	viewer.addShaderProgram(cubeMapShader);
	viewer.addShaderProgram(cartoonTextureShader);
	viewer.addShaderProgram(cartoonShader);

	// Materials
	MaterialPtr nolighting = Material::NoLighting();
	MaterialPtr nolightingblue = Material::NoLightingBlue();
	MaterialPtr white = Material::White();
	MaterialPtr water = Material::Water();
	MaterialPtr bark = Material::Bark();
	MaterialPtr sand = Material::Sand();
	MaterialPtr pureblack = Material::PureBlack();
	MaterialPtr black = Material::Black();
	MaterialPtr orange = Material::BrightOrange();
	MaterialPtr red = Material::BrightRed();
	MaterialPtr rock = Material::Rock();
	MaterialPtr darkgreen = Material::DarkGreen();
	MaterialPtr green = Material::Green();

	// Cubemap
	std::string cubemap_dir = "../Textures/skybox";
	auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
	viewer.addRenderable(cubemap);

	// Objects
	auto titre = add_object(viewer, "Titre", orange, cartoonShader);
	auto titre_blackdrop = add_object(viewer, "TitreBlackdrop", pureblack, cartoonShader);

	auto backdrop = add_textured_object(viewer, "FondIle", nolighting, "../Textures/FondIle.png", cartoonTextureShader);
	auto ground = add_object(viewer, "Ground", sand, cartoonShader);
	auto ground_coral = add_textured_object(viewer, "GroundCoral", nolighting, "../Textures/Corail.png", cartoonTextureShader);
	auto ground_rocks = add_object(viewer, "GroundRocks", rock, cartoonShader);
	auto ocean = add_object(viewer, "Ocean", water, cartoonShader);
	auto palmiers = add_object(viewer, "Palmiers", bark, cartoonShader);
	auto leaves = add_textured_object(viewer, "Leaves", white, "../Textures/Feuille.png", cartoonTextureShader);
	auto skipper = add_textured_object(viewer, "Skipper", white, "../Textures/Skipper.png", cartoonTextureShader);
	auto vietnam = add_object(viewer, "Red Beach Vietnam", red, cartoonShader);

	auto house2 = add_object(viewer, "maison.001", white, cartoonShader);
	auto clock = add_textured_object(viewer, "Clock", nolighting, "../Textures/clock.jpg", cartoonTextureShader);
	auto hour_hand = add_object(viewer, "Hours", white, cartoonShader, clock);
	auto minute_hand = add_object(viewer, "Minutes", white, cartoonShader, clock);
	auto bed_frame = add_object(viewer, "BedFrame", bark, cartoonShader);
	auto bed_sheets = add_object(viewer, "BedSheets", white, cartoonShader);
	auto sakado = add_object(viewer, "Sakado", darkgreen, cartoonShader);
	
	// Militaire
	auto soldats = add_object(viewer, "Soldats", green, cartoonShader);
	auto tank = add_object(viewer, "Tank", green, cartoonShader);
	auto tank1 = add_object(viewer, "Tank.001", green, cartoonShader);
	auto tank2 = add_object(viewer, "Tank.002", green, cartoonShader);
	auto tank3 = add_object(viewer, "Tank.003", green, cartoonShader);
	auto tank4 = add_object(viewer, "Tank.004", green, cartoonShader);
	auto avion = add_object(viewer, "Avion", green, cartoonShader);
	auto bombe = add_textured_object(viewer, "Bombe", white, "../Textures/Bombe.png", cartoonTextureShader);

	// Tortues marines
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

	// Tortues terrestres
	auto shell_ter = add_textured_object(viewer, "Carapace-ter", white, "../Textures/Tortue_orange.png", cartoonTextureShader);
	auto pat_ard = add_textured_object(viewer, "Pat-ArD", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter);
	auto pat_arg = add_textured_object(viewer, "Pat-ArG", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter);
	auto pat_avd = add_textured_object(viewer, "Pat-AvD", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter);
	auto pat_avg = add_textured_object(viewer, "Pat-AvG", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter);
	auto tete_ter = add_textured_object(viewer, "Tete-ter", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter);

	auto shell_ter2 = add_textured_object(viewer, "Carapace-ter.001", white, "../Textures/Tortue_orange.png", cartoonTextureShader);
	auto pat_ard2 = add_textured_object(viewer, "Pat-ArD.001", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter2);
	auto pat_arg2 = add_textured_object(viewer, "Pat-ArG.001", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter2);
	auto pat_avd2 = add_textured_object(viewer, "Pat-AvD.001", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter2);
	auto pat_avg2 = add_textured_object(viewer, "Pat-AvG.001", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter2);
	auto tete_ter2 = add_textured_object(viewer, "Tete-ter.001", white, "../Textures/Tortue_orange.png", cartoonTextureShader, shell_ter2);

	// Lights
	// Sun
	glm::vec3 d_direction = glm::normalize(glm::vec3(-1.0, -0.2, 1.0));
	glm::vec3 d_ambient(0.4, 0.4, 0.6), d_diffuse(1.5, 1.5, 1.5), d_specular(1.5, 1.5, 1.5);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	viewer.addDirectionalLight(directionalLight);
	// Point lights
	float p_constant = 1.0, p_linear = 5e-1, p_quadratic = 0.0;
	glm::vec3 p_position = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 p_ambient = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 p_diffuse = glm::vec3(3.0, 3.0, 3.0);
	glm::vec3 p_specular = glm::vec3(3.0, 3.0, 3.0);
	PointLightPtr title_light1 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse * 0.9f, p_specular * 0.9f, p_constant, p_linear, p_quadratic);
	title_light1->applyObjTransform("../ObjFiles/TitreLight1.obj");
	PointLightPtr title_light2 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse * 0.7f, p_specular * 0.7f, p_constant, p_linear, p_quadratic);
	title_light2->addKeyframesFromFile("../Animation/TitreLight2.animation", 0.0, false);
	p_diffuse = glm::vec3(3.0, 1.0, 1.0);
	PointLightPtr house_light1 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse * 0.4f, p_diffuse * 0.4f, p_constant, p_linear, p_quadratic);
	p_position = glm::vec3(-14.5, 0.5, 16.0);
	explosion_light = std::make_shared<PointLight>(p_position, p_ambient, p_ambient, p_ambient, p_constant, p_linear, p_quadratic);
	house_light1->applyObjTransform("../ObjFiles/HouseLight1.obj");
	viewer.addPointLight(title_light1);
	viewer.addPointLight(title_light2);
	viewer.addPointLight(house_light1);
	viewer.addPointLight(explosion_light);

	// Camera
	viewer.getCamera().setFov(0.5);
	viewer.getCamera().addKeyframesFromFile("../Animation/Camera.animation", 0.0, false);
	
	// Blue filter
	filter = add_object(viewer, "Filter", nolightingblue, cartoonShader); 

	// Soundtrack
	viewer.setSoundtrack("../tortuekaizen.wav");

	// Physical simulation
	DynamicSystemPtr system = std::make_shared<DynamicSystem>();
	system->setDt(1.0f / 240.0f);
	system->setSolver(std::make_shared<EulerExplicitSolver>());
	system->setRestitution(0.6f);

	std::vector<ParticlePtr> particles;
	const unsigned int count = 500u;
	for (unsigned int i = 0; i < count; ++i)
	{
		glm::vec3 pos = glm::ballRand(0.35f) + glm::vec3(-14.5f, -1.0f, 16.0f);
		pos.y += 0.35f;  // start close to ground cluster
		float const weight = std::max(glm::gaussRand(20.0f, 5.0f), 10.0f);
		float const radius = glm::linearRand(0.05f, 0.1f);
		ParticlePtr p = std::make_shared<Particle>(pos, glm::vec3(0.0f), weight, radius);
		particles.push_back(p);
		system->addParticle(p);
	}

	// Gravity
	ConstantForceFieldPtr gravity = std::make_shared<ConstantForceField>(particles, DynamicSystem::gravity);
	system->addForceField(gravity);

	// Damping to tame speeds and reduce tunneling
	DampingForceFieldPtr damping = std::make_shared<DampingForceField>(particles, 0.8f);
	system->addForceField(damping);

	// Radial Impulse Explosion Field
	glm::vec3 explosionPosition = glm::vec3(-14.5f, -2.0f, 16.0f);
	float explosionStrength = 4000.0f;      // 9000
	float explosionRadius = 5.0f;           // 5
	float explosionImpulseDuration = 0.2f;  // 0.2
	explosion = std::make_shared<RadialImpulseForceField>(particles,
	                                                      explosionPosition,
	                                                      explosionStrength,
	                                                      explosionRadius,
	                                                      explosionImpulseDuration,
	                                                      1.0f / 240.0f);
	system->addForceField(explosion);

	// Vortex Ring Mushroom Field
	glm::vec3 ringCenter = glm::vec3(-14.5f, -1.1f, 16.0f);
	float ringRadius = 10.0f;  // 20
	float ringHeight = 6.0f;   // 12
	float strength = 400.0f;   // 400
	float riseSpeed = 1.0f;    // 0.5
	float radialSpeed = 0.9f;  // 0.6
	mushroom = std::make_shared<MushroomForceField>(particles,
	                                                ringCenter,
	                                                ringRadius,
	                                                ringHeight,
	                                                strength,
	                                                riseSpeed,
	                                                radialSpeed);
	system->addForceField(mushroom);

	// Ground plane for collisions
	PlanePtr groundPlane = std::make_shared<Plane>(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.4f, 0.0f));
	system->addPlaneObstacle(groundPlane);

	// Renderables bound to the dynamic system
	auto systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
	systemRenderable->setStartTime(92.87f);
	viewer.addRenderable(systemRenderable);

	auto particlesRenderable = std::make_shared<ParticleListRenderable>(particleShader, particles, 12u, 16u);
	HierarchicalRenderable::addChild(systemRenderable, particlesRenderable);

	viewer.startAnimation();
}

int main()
{
	glm::vec4 background_color(0.8, 0.8, 0.8, 1);
	Viewer viewer(background_color);
	viewer.setTimeFactor(1.004f); // Correct weird audio sync issue
	RadialImpulseForceFieldPtr explosion;
	MushroomForceFieldPtr mushroom;
	PointLightPtr explosion_light;
	LightedMeshRenderablePtr filter;
	initialize_scene(viewer, explosion, mushroom, explosion_light, filter);
	filter->setLocalTransform(getTranslationMatrix(0.0, 0.0, -0.1));

	glm::vec3 explosion_color = glm::vec3(3.0, 2.0, 1.0);
	float explosion_strength = 0.0f;
	bool explosionTriggered = false;
	while (viewer.isRunning())
	{
		if (viewer.getTime() >= 100.0f) // End
		{
			break;
		}
		viewer.handleEvent();
		viewer.animate();
		if (viewer.getTime() >= 95.87f)
		{
			if (!explosionTriggered) {
				if (explosion)
					explosion->trigger();
				if (mushroom)
					mushroom->trigger();
				explosionTriggered = true;
			}
			if (explosion_strength >= 0.0f) {
				explosion_strength = 10.0f - (viewer.getTime() - 95.87f) * 5.0f;
				explosion_light->setDiffuse(explosion_color * std::max(explosion_strength, 0.0f));
				explosion_light->setSpecular(explosion_color * std::max(explosion_strength, 0.0f));
			}
		}
		if (viewer.getTime() >= 2.6666f && viewer.getTime() <= 66.7f) {
			filter->setGlobalTransform(viewer.getCamera().getGlobalTransform());
		} else {
			filter->setGlobalTransform(getTranslationMatrix(0.0, -50000.0, 0.0)); // Hide filter by moving it very very far
		}

		viewer.draw();
		viewer.display();
	}

	return EXIT_SUCCESS;
}
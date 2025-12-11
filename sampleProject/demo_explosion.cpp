#include <FrameRenderable.hpp>
#include <Plane.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <ShaderProgram.hpp>
#include <Viewer.hpp>
#include <algorithm>
#include <dynamics/ConstantForceField.hpp>
#include <dynamics/DampingForceField.hpp>
#include <dynamics/DynamicSystem.hpp>
#include <dynamics/DynamicSystemRenderable.hpp>
#include <dynamics/EulerExplicitSolver.hpp>
#include <dynamics/MushroomForceField.hpp>
#include <dynamics/ParticleListRenderable.hpp>
#include <dynamics/RadialImpulseForceField.hpp>
#include <glm/gtc/random.hpp>

int main()
{
	glm::vec4 background_color(0.1f, 0.1f, 0.12f, 1.0f);
	Viewer viewer(1280, 720, background_color);

	// Paths are relative to sampleProject/build at runtime (run.sh cd there)
	ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
	                                                              "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
	ShaderProgramPtr instancedShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/instancedVertex.glsl",
	                                                                   "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
	viewer.addShaderProgram(flatShader);
	viewer.addShaderProgram(instancedShader);

	// Frame for orientation
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	// Dynamic system setup
	DynamicSystemPtr system = std::make_shared<DynamicSystem>();
	system->setDt(1.0f / 240.0f);
	system->setSolver(std::make_shared<EulerExplicitSolver>());
	system->setRestitution(0.6f);

	std::vector<ParticlePtr> particles;
	const unsigned int count = 500u;
	for (unsigned int i = 0; i < count; ++i)
	{
		glm::vec3 pos = glm::ballRand(0.35f);
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
	glm::vec3 explosionPosition = glm::vec3(0.0f, -0.5f, 0.0f);  // (0, -0.5, 0)
	float explosionStrength = 9000.0f;                           // 9000
	float explosionRadius = 5.0f;                                // 5
	float explosionImpulseDuration = 0.2f;                       // 0.2
	RadialImpulseForceFieldPtr explosion = std::make_shared<RadialImpulseForceField>(particles,
	                                                                                 explosionPosition,
	                                                                                 explosionStrength,
	                                                                                 explosionRadius,
	                                                                                 explosionImpulseDuration,
	                                                                                 1.0f / 240.0f);
	system->addForceField(explosion);

	// Vortex Ring Mushroom Field
	glm::vec3 ringCenter = glm::vec3(0.0f, 0.0f, 0.0f);  // (0, 0, 0)
	float ringRadius = 20.0f;                            // 20
	float ringHeight = 12.0f;                            // 12
	float strength = 400.0f;                             // 400
	float riseSpeed = 0.5f;                              // 0.5
	float radialSpeed = 0.6f;                            // 0.6
	MushroomForceFieldPtr mushroom = std::make_shared<MushroomForceField>(particles,
	                                                                      ringCenter,
	                                                                      ringRadius,
	                                                                      ringHeight,
	                                                                      strength,
	                                                                      riseSpeed,
	                                                                      radialSpeed);
	system->addForceField(mushroom);

	// Ground plane for collisions
	PlanePtr ground = std::make_shared<Plane>(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	system->addPlaneObstacle(ground);

	// Renderables bound to the dynamic system
	auto systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
	viewer.addRenderable(systemRenderable);

	auto particlesRenderable = std::make_shared<ParticleListRenderable>(instancedShader, particles, 12u, 16u);
	HierarchicalRenderable::addChild(systemRenderable, particlesRenderable);

	viewer.startAnimation();

	bool isExploding = false;
	sf::Clock clock;
	sf::Time explosionStartTime;
	float explosionDuration = 2.0f;

	while (viewer.isRunning())
	{
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();

		// Trigger explosion on 'B' key press
		if (!isExploding && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::B))
		{
			explosion->trigger();
			mushroom->trigger();
			isExploding = true;
			explosionStartTime = clock.getElapsedTime();
		}

		// Stop explosion after duration
		if (isExploding && (clock.getElapsedTime() - explosionStartTime).asSeconds() > explosionDuration)
		{
			mushroom->stop();
			explosion->stop();
			isExploding = false;
		}
	}

	return EXIT_SUCCESS;
}

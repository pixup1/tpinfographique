#ifndef MUSHROOM_FORCE_FIELD_HPP
#define MUSHROOM_FORCE_FIELD_HPP

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "ForceField.hpp"
#include "Particle.hpp"

/** @brief Force field simulating a Vortex Ring (Mushroom Cloud).
 *
 * Creates a toroidal circulation around a ring at (radius, height).
 * Physics:
 *  - Inside the ring (r < R): Upward flow
 *  - Above the ring (y > H): Outward flow
 *  - Outside the ring (r > R): Downward flow
 *  - Below the ring (y < H): Inward flow
 */
class MushroomForceField : public ForceField
{
   public:
	/**
	 * @param center Origin of the explosion (x, z) and base y.
	 * @param ringRadius Radius of the mushroom cap (R).
	 * @param ringHeight Height of the mushroom cap center relative to center.y.
	 * @param strength Circulation strength (rotation speed).
	 * @param riseSpeed Global upward drift added to the vortex.
	 */
	MushroomForceField(const std::vector<ParticlePtr>& particles,
	                   const glm::vec3& center,
	                   float ringRadius,
	                   float ringHeight,
	                   float strength,
	                   float riseSpeed,
	                   float radialSpeed);

	void setCenter(const glm::vec3& c);
	void setRingRadius(float r);
	void setRingHeight(float h);
	void setStrength(float s);

	/**
	 * @brief Activate the force field.
	 */
	void trigger();

	/**
	 * @brief Deactivate the force field.
	 */
	void stop();

	/**
	 * @brief Check if the force field is currently active.
	 * @return true if active, false otherwise.
	 */
	bool isActive() const;

   private:
	void do_addForce() override;

	std::vector<ParticlePtr> m_particles;
	glm::vec3 m_center;
	float m_ringRadius;
	float m_ringHeight;
	float m_strength;
	float m_riseSpeed;
	float m_radialSpeed;
	bool m_active;
};

using MushroomForceFieldPtr = std::shared_ptr<MushroomForceField>;

#endif  // MUSHROOM_FORCE_FIELD_HPP

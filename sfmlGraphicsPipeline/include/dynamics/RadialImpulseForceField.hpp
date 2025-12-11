#ifndef RADIAL_IMPULSE_FORCE_FIELD_HPP
#define RADIAL_IMPULSE_FORCE_FIELD_HPP

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "ForceField.hpp"
#include "Particle.hpp"

/**@brief Radial blast force field.
 *
 * Applies a short-lived radial impulse centered at a position. The impulse
 * decays over time (duration) and with distance (radius). Call trigger() to
 * arm the blast; it will auto-disable when the duration has elapsed.
 */
class RadialImpulseForceField : public ForceField
{
   public:
	RadialImpulseForceField(const std::vector<ParticlePtr>& particles,
	                        const glm::vec3& center,
	                        float strength,
	                        float radius,
	                        float duration,
	                        float dt);

	/**
	 * @brief Activate the force field.
	 */
	void trigger();

	/**
	 * @brief Deactivate the force field.
	 */
	void stop();

	void setCenter(const glm::vec3& center);
	void setStrength(float strength);
	void setRadius(float radius);
	void setDuration(float duration);
	void setDt(float dt);

	/**
	 * @brief Check if the force field is currently active.
	 * @return true if active, false otherwise.
	 */
	bool isActive() const;

   private:
	void do_addForce() override;

	std::vector<ParticlePtr> m_particles;
	glm::vec3 m_center;
	float m_strength;
	float m_radius;
	float m_duration;
	float m_dt;
	float m_remainingTime;
	bool m_active;
};

typedef std::shared_ptr<RadialImpulseForceField> RadialImpulseForceFieldPtr;

#endif  // RADIAL_IMPULSE_FORCE_FIELD_HPP

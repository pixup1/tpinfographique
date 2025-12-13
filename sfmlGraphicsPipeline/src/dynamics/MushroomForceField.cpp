#include "../../include/dynamics/MushroomForceField.hpp"

#include <algorithm>
#include <cmath>

MushroomForceField::MushroomForceField(const std::vector<ParticlePtr>& particles,
                                       const glm::vec3& center,
                                       float ringRadius,
                                       float ringHeight,
                                       float strength,
                                       float riseSpeed,
                                       float radialSpeed)
    : m_particles(particles),
      m_center(center),
      m_ringRadius(std::max(0.1f, ringRadius)),
      m_ringHeight(ringHeight),
      m_strength(strength),
      m_riseSpeed(riseSpeed),
      m_radialSpeed(radialSpeed),
      m_active(false)
{
}

void MushroomForceField::trigger()
{
	m_active = true;
}

void MushroomForceField::stop()
{
	m_active = false;
}

bool MushroomForceField::isActive() const
{
	return m_active;
}

void MushroomForceField::setCenter(const glm::vec3& c)
{
	m_center = c;
}
void MushroomForceField::setRingRadius(float r)
{
	m_ringRadius = std::max(0.1f, r);
}
void MushroomForceField::setRingHeight(float h)
{
	m_ringHeight = h;
}
void MushroomForceField::setStrength(float s)
{
	m_strength = s;
}

void MushroomForceField::do_addForce()
{
	if (!m_active)
		return;

	// Vortex Ring Physics
	// We simulate a toroidal vortex centered at (m_center.x, m_center.y + m_ringHeight, m_center.z)
	// with major radius m_ringRadius.

	float coreY = m_center.y + m_ringHeight;
	// Sigma controls the thickness of the vortex core influence
	float sigma = m_ringRadius * 0.5f;
	float sigmaSq = sigma * sigma;

	for (auto& p : m_particles)
	{
		glm::vec3 pos = p->getPosition();
		glm::vec3 local = pos - m_center;

		// Cylindrical coordinates relative to axis
		float r = glm::length(glm::vec2(local.x, local.z));

		// Vector from vortex core to particle in the Poloidal plane (r, y)
		// Core is at (R, H) in (r, y) space
		float dr = r - m_ringRadius;
		float dy = pos.y - coreY;

		// Distance squared to the vortex filament
		float distSq = dr * dr + dy * dy;

		// Circulation direction: Tangent to the circle around the core
		// We want flow to go UP in the center (r < R) -> dy positive
		// Vector from core: (dr, dy)
		// Perpendicular (Tangent): (dy, -dr)
		// Check:
		//  Inside (dr < 0): vertical component is -(-dr) = +dr (positive magnitude) -> Up. Correct.
		//  Top (dy > 0): radial component is dy (positive) -> Out. Correct.

		glm::vec2 tangent(dy, -dr);

		// Gaussian falloff for the vortex strength
		float intensity = m_strength * std::exp(-distSq / sigmaSq);

		// Radial and Vertical force components
		float fRadial = tangent.x * intensity;
		float fVertical = tangent.y * intensity;

		// Add radial inflow/outflow speed
		fRadial *= m_radialSpeed;

		// Add global rise speed (buoyancy)
		fVertical *= m_riseSpeed;

		// Convert back to 3D Cartesian
		glm::vec3 force(0.0f);
		if (r > 1e-5f)
		{
			force.x = (local.x / r) * fRadial;
			force.z = (local.z / r) * fRadial;
		}
		force.y = fVertical;

		p->incrForce(force);
	}
}

#include "../../include/dynamics/RadialImpulseForceField.hpp"

#include <algorithm>
#include <cmath>

RadialImpulseForceField::RadialImpulseForceField(const std::vector<ParticlePtr>& particles,
                                                 const glm::vec3& center,
                                                 float strength,
                                                 float radius,
                                                 float duration,
                                                 float dt)
    : m_particles(particles),
      m_center(center),
      m_strength(strength),
      m_radius(std::max(0.0f, radius)),
      m_duration(std::max(0.0f, duration)),
      m_dt(std::max(0.0f, dt)),
      m_remainingTime(0.0f),
      m_active(false)
{
}

void RadialImpulseForceField::trigger()
{
	if (m_radius <= 0.0f || m_duration <= 0.0f)
	{
		m_active = false;
		m_remainingTime = 0.0f;
		return;
	}
	m_remainingTime = m_duration;
	m_active = true;
}

void RadialImpulseForceField::stop()
{
	m_active = false;
	m_remainingTime = 0.0f;
}

void RadialImpulseForceField::setCenter(const glm::vec3& center)
{
	m_center = center;
}

void RadialImpulseForceField::setStrength(float strength)
{
	m_strength = strength;
}

void RadialImpulseForceField::setRadius(float radius)
{
	m_radius = std::max(0.0f, radius);
}

void RadialImpulseForceField::setDuration(float duration)
{
	m_duration = std::max(0.0f, duration);
}

void RadialImpulseForceField::setDt(float dt)
{
	m_dt = std::max(0.0f, dt);
}

bool RadialImpulseForceField::isActive() const
{
	return m_active;
}

void RadialImpulseForceField::do_addForce()
{
	if (!m_active || m_remainingTime <= 0.0f)
	{
		m_active = false;
		return;
	}

	// Linear temporal falloff
	float timeFactor = (m_duration > 0.0f) ? (m_remainingTime / m_duration) : 1.0f;

	for (auto& p : m_particles)
	{
		glm::vec3 dir = p->getPosition() - m_center;
		float dist = glm::length(dir);
		if (dist < 1e-5f)
		{
			continue;
		}

		dir /= dist;
		// Gaussian falloff: smooth decay without hard threshold
		float spatialFactor = std::exp(-(dist * dist) / (m_radius * m_radius));
		glm::vec3 force = dir * (m_strength * spatialFactor * timeFactor);
		p->incrForce(force);
	}

	m_remainingTime -= m_dt;
	if (m_remainingTime <= 0.0f)
	{
		stop();
	}
}

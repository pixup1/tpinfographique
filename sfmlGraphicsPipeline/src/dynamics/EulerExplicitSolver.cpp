#include "../../include/dynamics/EulerExplicitSolver.hpp"

EulerExplicitSolver::EulerExplicitSolver()
{
}

EulerExplicitSolver::~EulerExplicitSolver()
{
}

void EulerExplicitSolver::do_solve(const float& dt, std::vector<ParticlePtr>& particles)
{
	for (ParticlePtr p : particles)
	{
		if (!p->isFixed())
		{
			// Update particle velocity
			p->setVelocity(p->getVelocity() + (1 / p->getMass()) * dt * p->getForce());
			// Update particle position
			p->setPosition(p->getPosition() + dt * p->getVelocity());
		}
	}
}

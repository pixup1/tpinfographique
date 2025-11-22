#import "@preview/silky-report-insa:0.5.2": *
#show: doc => insa-report(
    id: 5,
    pre-title: "Informatique Graphique",
    title: "Animation procédurale",
    authors: [
        *GRAINDORGE Amance*

        *VERNET Hector*
        #v(70pt)
    ],
    date: "17/11/2025",
    insa: "rennes",
    doc,
)

#show link: it => underline(offset: 2pt)[#it]

#show raw.where(block: true): it => {
    if it.lines.first().count > 1 {
        block(stroke: 0.5pt + black, inset: 5pt, width: 100%, grid(
            columns: (auto, auto),
            column-gutter: 2em,
            row-gutter: par.leading,
            align: (right, raw.align),
            ..for line in it.lines {
                (
                    text(fill: luma(150))[#line.number],
                    line.body,
                )
            },
        ))
    } else { it.lines.first() }
}

= Introduction

Ce TP a pour objectif d'utiliser un système de simulation simple pour animer une scène.
Ce système se base sur l'application de différents types de force à des particules (gravité, ressort, collision).
Nous commencerons par mettre en place le calcul du déplacement des particules — qui sera la base de notre simulation — puis nous ajouterons de nouvelles interactions entre particules, les ressorts et les collisions.
Enfin, nous observerons l'utilisation de la simulation dans une scène plus complexe avant de l'appliquer à notre projet.

= Exercice 1

Dans ce premier exercice, nous allons écrire la méthode `EulerExplicitSolver::do_solve(…)` qui calcule la vitesse et la position de toutes les particules après un temps `dt` en fonction de son état actuel, en utilisant les équations de mouvement.
La voici complétée :

```cpp
void EulerExplicitSolver::do_solve(
  const float& dt,
  std::vector<ParticlePtr>& particles
)
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
```

Une fois la méthode fonctionnelle, on obtient le résultat suivant :

#figure(
    grid(
        columns: (1fr, 1fr, 1fr),
        image("images/TP5/Ex1-1.png"), image("images/TP5/Ex1-2.png"), image("images/TP5/Ex1-3.png"),
    ),
    caption: "Implémentation du calcul du mouvement des particules",
)

= Exercice 2

Ce deuxième exercice a pour objectif de simuler un objet composé de plusieurs particules. Pour cela, chaque particule composant l'objet sera consdérée comme liée au autres par un ressort, et subira des forces en conséquence. Nous avons donc écrit la méthode `SpringForceField::do_addForce()`, qui est chargée de calculer les forces à appliquer aux deux extrémités de chaque ressort :

```cpp
void SpringForceField::do_addForce()
{
	// Compute displacement vector
	glm::vec3 displacement = m_p1->getPosition() - m_p2->getPosition();

	// Compute displacement length
	float displacementLength = glm::length(displacement);

	// Compute spring force corresponding to the displacement
	// If the displacement is measurable by the computer (otherwise no force)
	if (displacementLength > std::numeric_limits<float>::epsilon())
	{
		glm::vec3 normalizedDisplacement = glm::normalize(displacement);
		glm::vec3 idealForce = -m_stiffness * (displacementLength - m_equilibriumLength) * normalizedDisplacement;
		glm::vec3 relativeVelocity = m_p1->getVelocity() - m_p2->getVelocity();
		float dampingFactor = glm::dot(relativeVelocity, normalizedDisplacement);
		glm::vec3 dampedForce = -m_damping * dampingFactor * normalizedDisplacement;
		glm::vec3 totalForce = idealForce + dampedForce;

		m_p1->setForce(m_p1->getForce() + totalForce);
		m_p2->setForce(m_p2->getForce() - totalForce);
	}
}
```

L'exemple de cet exercice est un filet composé de particules reliées entre elles par des ressorts. Lorsque l'on lance la simulation une première fois sans changer de paramètre, on constate que le filet reste constament en mouvement, ce qui n'est pas réaliste. On change donc les valeurs de l'amortissement (`damping`) global (→10) et celui des ressort (→ 50). L'amortissement simule des pertes d'énergie présentes dans la réalité, comme les frottements de l'air (global) ou l'échauffement (ressort), ce qui va permettre à notre système de se stabiliser après un certain temps :

#figure(
    grid(
        columns: (1fr, 1fr),
        image("images/TP5/Ex2-1.png"), image("images/TP5/Ex2-2.png"),
        image("images/TP5/Ex2-3.png"), image("images/TP5/Ex2-4.png"),
    ),
    caption: "Simulation de filet avec ressorts",
)

= Exercice 3

L'objectif de ce troisième exercice est d'implémenter les collisions entre plusieurs particules. Les collisions entre deux particules étant déjà fonctionnelles, nous nous sommes concentrés sur l'interaction entre une particule et un plan. Nous avons donc complété la méthode `ParticlePlaneCollision::do_solveCollision()` qui calcule la nouvelle position et vitesse d'une particule après une collision avec un plan infini. Voici le code complété :

```cpp
void ParticlePlaneCollision::do_solveCollision()
{
	// Don't process fixed particles (Let's assume that the ground plane is fixed)
	if (m_particle->isFixed())
		return;

	// Compute interpenetration distance
	float planeParticleDist = glm::dot(
        m_particle->getPosition(),
        m_plane->normal()
    ) - m_plane->distanceToOrigin();
	float interpenetrationDist = m_particle->getRadius() - planeParticleDist;

	// No collision
	if (interpenetrationDist <= 0)
		return;

	// Project the particle on the plane
	glm::vec3 proj = m_plane->projectOnPlane(m_particle->getPosition());
	m_particle->setPosition(proj + m_particle->getRadius() * m_plane->normal());

	// Compute post-collision velocity
	m_particle->setVelocity(
		m_particle->getVelocity()
		- (1.0f + m_restitution) * glm::dot(m_particle->getVelocity(),
		m_plane->normal()) * (m_plane->normal())
	);
}
```
Nous avons également complété la fonction de test de collision `testParticlePlane()` :
```cpp
bool testParticlePlane(const ParticlePtr& particle, const PlanePtr& plane)
{
	float particlePlaneDist = glm::dot(
		particle->getPosition(), plane->normal()
	) - plane->distanceToOrigin();
	return std::abs(particlePlaneDist) <= particle->getRadius();
}
```

On teste ensuite notre implémentation avec une scène simple où une particule rebondit sur une particule fixe, et une autre particule rebondit sur un plan fixe. On ajoute également une composante horizontale à la vitesse initiale de l'une des particules pour vérifier le réalisme des interactions. Voici le résultat obtenu :

#figure(
    grid(
        columns: (1fr, 1fr),
        image("images/TP5/Ex3-1.png"), image("images/TP5/Ex3-2.png"),
        image("images/TP5/Ex3-3.png"), image("images/TP5/Ex3-4.png"),
    ),
    caption: "Simulation de collisions entre particules et plan",
)

On voit que les collisions sont bien gérées, avec un rebond réaliste des particules. Cependant, dans le cas d'un grand nombre d'interactions, des problèmes d'interpenetration peuvent subvenir, car la position des particules n'est corrigée qu'une seule fois par itération. Des particules peuvent donc se retrouver à l'intérieur d'autres particules _après_ leur calcul de collision.

= Exercice 4

Ce dernier exercice nous propose de tester notre système de simulations dans une scène plus complexe, composée d'une table de billard et de deux particules. L'une des particules peut être controlée au clavier par l'utilisateur, en utilisant la classe `ControlledForceFieldRenderable`, tandis que l'autre réagit seulement aux collisions. Voici le résultat obtenu :

#figure(
    grid(
        columns: (1fr, 1fr),
        image("images/TP5/Ex4-1.png"), image("images/TP5/Ex4-2.png"),
        image("images/TP5/Ex4-3.png"), image("images/TP5/Ex4-4.png"),
    ),
    caption: "Simulation d'une table de billard avec particules contrôlables",
)

= Conclusion

Ce TP nous a permis de mettre en place un système de simulation physique basé sur l'application de forces à des particules. Nous avons implémenté le calcul du mouvement des particules, la gestion des ressorts entre particules, ainsi que les collisions entre particules et plans. Ces éléments constituent une base solide pour des simulations plus complexes, et pourront être utilisés dans notre projet final.

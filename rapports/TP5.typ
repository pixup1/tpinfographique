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

= Exercice 4

= Projet

= Conclusion

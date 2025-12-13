#import "@preview/silky-report-insa:0.5.2": *
#show: doc => insa-document(
    "light",
    cover-top-left: [
        #set text(27pt)
        *Informatique Graphique - Projet*
        #set text(24pt)
        Tortue Kaizen
    ],
    cover-middle-left: [
        #v(-70pt)
        *GRAINDORGE Amance*
        
        *VERNET Hector*
    ],
    cover-bottom-right: "",
    page-header: [
        Projet d'Informatique Graphique
        #h(1fr)
        15/12/2025
    ],
    doc,
)

#show link: it => underline(offset: 2pt)[#it]

#show raw.where(block: true): it => {
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
}

= Introduction

Dans ce rapport, nous allons détailler la réalisation de notre projet d'informatique graphique et plus spécifiquement, les éléments techniques que nous avons ajouté au code fourni pour obtenir le résultat final.

= Animation

== Modes d'interpolation

== Import depuis Blender

= Matériaux avec transparence

Pour avoir des objets transparents, ce qui était une nécessité pour une scène en particulier dans laquelle on voit des tortues sur la plage depuis le dessous de la mer, nous avons ajouté une propriété `alpha` à la classe `Material`. Une fois cette propriété passée aux shaders avec `program->getUniformLocation("material.alpha")` et `glUniform1f` comme pour les autres propriétés, on peut l'utiliser à la fin du fragment shader ainsi :

```glsl
outColor = vec4(tmpColor,material.alpha);
```

Cependant, il reste un problème : si un objet transparent est dessiné avant un objet opaque qui se trouve derrière lui, l'objet opaque ne sera pas visible. La solution est de déterminer quels objets sont transparents et de les dessiner en dernier. On rajoute donc quelques lignes dans `Viewer::draw()` :

```cpp
// Separate opaque and transparent objects
std::vector<RenderablePtr> opaque_renderables;
std::vector<RenderablePtr> transparent_renderables;

for (const RenderablePtr &r : m_renderables)
{
	LightedMeshRenderablePtr lm = std::dynamic_pointer_cast<LightedMeshRenderable>(r);
	if (lm != nullptr && lm->getMaterial()->alpha() < 1.0f)
	{
		transparent_renderables.push_back(r);
	}
	else
	{
		opaque_renderables.push_back(r);
	}
}
	
// We draw the opaque objects first so that they can always appear behind the transparent ones
std::vector<RenderablePtr> sorted_renderables;
sorted_renderables.insert(sorted_renderables.end(), opaque_renderables.begin(), opaque_renderables.end());
sorted_renderables.insert(sorted_renderables.end(), transparent_renderables.begin(), transparent_renderables.end());
```

On peut maintenant créer le matériau `Water` avec un `alpha` de 0.7, et la scène s'affiche comme prévu.

= Post-processing

= Simulation

La scène de fin du projet nécessitait de montrer une explosion, que nous avons décidé de simuler avec des particules. On utilise donc les classes `Particle` et `ParticleListRenderable` pour gérer et afficher les particules. Les particules sont initialiées avec une position aléatoire dans une sphère, une vitesse nulle, un poids suivant une distribution gaussienne et un rayon aléatoire entre 0.05 et 0.1.

```cpp
std::vector<ParticlePtr> particles;
const unsigned int count = 500u;
for (unsigned int i = 0; i < count; ++i)
{
  glm::vec3 pos = glm::ballRand(0.35f) + glm::vec3(-14.5f, -1.0f, 16.0f);
  pos.y += 0.35f;
  float const weight = std::max(glm::gaussRand(20.0f, 5.0f), 10.0f);
  float const radius = glm::linearRand(0.05f, 0.1f);
  ParticlePtr p = std::make_shared<Particle>(pos, glm::vec3(0.0f), weight, radius);
  particles.push_back(p);
  system->addParticle(p);
}
```

L'intérêt de ces paramètres initiaux est que les particules se répartissent de manière naturelle sur le plan de la simulation.

La classe `DynamicSystemRenderable` a été modifié pour pouvoir spécifier un temps de début de la simulation :

```cpp
auto systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
systemRenderable->setStartTime(92.87f);
viewer.addRenderable(systemRenderable);
```

L'explosion en elle-même est simulée avec deux champs de force, en plus de la gravité : un champ de force radial qui pousse les particules vers l'extérieur par rapport à un point, et un champ de force qui donne une forme de champignon à l'explosion. Les deux nouvelles classes `RadialImpulseForceField` et `MushroomForceField` héritent de `ForceField`, et possèdent leur propre implémentation de la méthode `do_addForce()`.

*RadialImpulseForceField*

```cpp
void RadialImpulseForceField::do_addForce()
{
	if (!m_active || m_remainingTime <= 0.0f)
	{
		m_active = false;
		return;
	}

	// La force diminue linéairement avec le temps
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
		// La force diminue de façon gaussienne avec la distance 
    // pour éviter des effets trop brusques
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
```

*MushroomForceField*

```cpp
void MushroomForceField::do_addForce()
{
	if (!m_active)
		return;

  // On simule un vortex toroïdal centré en (m_center.x, m_center.y + m_ringHeight, m_center.z) avec un rayon majeur m_ringRadius.

	float coreY = m_center.y + m_ringHeight;
	// Sigma contrôle la "largeur" du vortex
	float sigma = m_ringRadius * 0.5f;
	float sigmaSq = sigma * sigma;

	for (auto& p : m_particles)
	{
		glm::vec3 pos = p->getPosition();
		glm::vec3 local = pos - m_center;

		// Distance radiale dans le plan horizontal (x, z)
		float r = glm::length(glm::vec2(local.x, local.z));

		// Vecteur depuis le centre du vortex dans le plan (r, y)
		float dr = r - m_ringRadius;
		float dy = pos.y - coreY;

		// Distance au carré depuis le centre du vortex
		float distSq = dr * dr + dy * dy;

    // Direction de la circulation : tangente au cercle autour du noyau
    // On veut que le flux monte au centre (r < R) -> dy positif
    // Vecteur depuis le noyau : (dr, dy)
    // Perpendiculaire (tangente) : (dy, -dr)
		glm::vec2 tangent(dy, -dr);

		// La force diminue de façon gaussienne avec la distance au centre du vortex
		float intensity = m_strength * std::exp(-distSq / sigmaSq);

		// Composantes radiale et verticale
		float fRadial = tangent.x * intensity;
		float fVertical = tangent.y * intensity;

		// Application d'un facteur de vitesse
		fRadial *= m_radialSpeed;
		fVertical *= m_riseSpeed;

		// Conversion en repère cartésien
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
```

Pour se représenter plus facilement l'effet de ce champ de force :

#figure(
  image("images/Projet/vortex_toroidal.png"),
  caption: "Graphique illustrant l'influence du champ de force en forme de vortex toroïdal sur les particules."
)

Les deux champs de force possèdent également une méthode `trigger()` et une méthode `stop()` pour démarrer et arrêter leur effet. On peut ainsi déclencher l'explosion à un moment précis dans l'animation. Nous avons ensuite ajusté les paramètres des champs de force pour obtenir un rendu satisfaisant.

Nous nous sommes cependant heurtés à une limite de performance : avec un grand nombre de particules, la simulation devenait très lente à cause du calcul des interactions entre particules. Nous avons donc limité le nombre de particules à 500 pour maintenir une fluidité acceptable. Une amélioration possible serait d'utiliser le GPU pour effectuer les calculs de ces interactions, ou alors de trouver une méthode plus efficace pour répartir les particules de façon naturelle et désactiver les collisions.

= Audio

Pour l'audio du projet, nous utilisons l'utilitaire linux `aplay` qui joue un fichier `.wav` contenant toute notre bande son. Cette commande est lancée automatiquement dans un nouveau processus au lancement de l'animation par `Viewer.cpp`.

```cpp
void Viewer::startAnimation()
{
	m_lastSimulationTimePoint = clock::now();
	m_animationIsStarted = true;
	if (!soundtrack_path.empty()) {
		std::string cmd = "aplay \"" + soundtrack_path + "\" &";
		std::system(cmd.c_str());
	}
}
```

= Conclusion
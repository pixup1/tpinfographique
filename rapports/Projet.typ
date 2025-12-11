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
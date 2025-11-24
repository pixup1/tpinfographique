#import "@preview/silky-report-insa:0.5.2": *
#show: doc => insa-report(
  id: 5,
  pre-title: "Informatique Graphique",
  title: "Illumination locale",
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

Dans ce TP, nous allons implémenter le modèle de Phong pour illuminer notre scène dynamiquement. Ce modèle comprend des composantes ambiante, diffuse et spéculaire, qui permettent d'approximer simplement l'interaction de la lumière avec les surfaces, tout en exposant beaucoup de paramètres nous donnant un contrôle artistique sur le résultat. Ce modèle sera implémenté pour différentes sources lumineuses : directionnelle, ponctuelle et spot.

= Exercice 1

Cet exercice consiste simplement à vérifier que l'implémentation déjà présente fonctionne correctement. Dans `phongFragment.glsl`, pour le calcul de la lumière directionnelle (une lumière infiniment distante aux rayons parallèles, le cas le plus simple à traiter), on retrouve les trois composantes du modèle de Phong :

- La composante ambiante, dépendant simplement de deux constantes :
#raw("vec3 ambient = light.ambient \* material.ambient ;", lang: "glsl", block: true) // tabernak ça marche pas

- La composante diffuse, facteur du produit scalaire entre la normale et la direction de la lumière :
```glsl
float diffuse_factor = max(dot(surfel_normal, surfel_to_light), 0.0);
vec3 diffuse = diffuse_factor * light.diffuse * material.diffuse ;
```

- La composante spéculaire, 
```glsl
vec3 reflect_direction = reflect(-surfel_to_light, surfel_normal);
float specular_dot = clamp(dot(surfel_to_camera, reflect_direction), 0, 1);
float specular_factor = pow(specular_dot, material.shininess);
vec3 specular = specular_factor * light.specular * material.specular;
```

La couleur finale est obtenue en faisant la somme de ces trois composantes.

= Exercice 2

Dans cet exercice, nous allons ajouter le concept d'atténuation de la lumière, qui fait qu'elle diminue de manière proportionnelle au carré de la distance parcourue. Cette atténuation dépend également de trois coefficients propres (constant, linéaire et quadratique) qui permettent de contrôler la rapidité de cette diminution.

La formule est la suivante :
```glsl
float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
```

On l'implémente pour les lumières ponctuelles et spot uniquement, étant donné que les lumières directionnelles sont supposées infiniment éloignées et uniformes sur toute la scène (il s'agira typiquement du soleil).

Voici un avant/après de l'effet d'atténuation avec la scène d'example :

#figure(grid(
  columns: 2,
  image("images/TP6/attenuation_avant.png"),
  image("images/TP6/attenuation_apres.png")
  ),
  caption: "Effet de l'atténuation sur la scène d'exemple"
)

= Exercice 3

Dans cet exercice, nous allons implémenter l'"ouverture" des lumières spot, qui permet de définir un cône d'illumination. Dans un premier temps, nous n'utiliserons q'un seul seuil, ce qui donnera une coupure nette de la lumière en dehors du cône.

La formule est donc celle-ci (`light.innerCutOff` étant l'angle d'ouverture, ou le seuil) :
```glsl
float intensity = max(dot(surfel_to_light, -light.spotDirection), 0.0) < cos(light.innerCutOff / 2) ? 0.0 : 1.0;
```

Cette intensité est ensuite multipliée à la lumière diffuse et spéculaire.

Voici le spot formé :

#figure(image("images/TP6/spot1.png", width: 60%), caption: "Lumière spot avec un seul seuil")

= Exercice 4

À présent, nous allons ajouter un deuxième seuil pour adoucir la coupure de la lumière spot. Nous utilisons pour cela deux angles : un angle interne "`innerCutOff`" et un angle externe "`outerCutOff`".

Voici la formule de l'intensité révisée :
```glsl
float cos_phi = max(dot(surfel_to_light, -light.spotDirection), 0.0);
float intensity = clamp((cos_phi - cos(light.outerCutOff / 2))/(cos(light.outerCutOff / 2) - cos(light.innerCutOff / 2)),0,1);
```

Avec cette nouvelle formule, on obtient une transition plus douce :

#figure(image("images/TP6/spot2.png", width: 60%), caption: "Lumière spot avec transition douce")

= Exercice 5

Dans cet exercice, nous allons profiter du fait que les lumières héritent de la classe `KeyframedHierarchicalRenderable` pour les animer dans notre scène de test.

On ajoute un source lumineuse en mouvement dans `practical6.cpp` :
```cpp
p_position = glm::vec3(0.0, 0.0, 0.0);
p_ambient = glm::vec3(0.0, 0.0, 0.0);
p_diffuse = glm::vec3(2.0, 2.0, 2.0);
p_specular = glm::vec3(2.0, 2.0, 2.0);
p_constant = 1.0;
p_linear = 5e-1;
p_quadratic = 0;
PointLightPtr pointLight3 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse, p_specular, p_constant, p_linear, p_quadratic);
pointLight3->addGlobalTransformKeyframe(getTranslationMatrix(glm::vec3(5.0, 0.0, 2.0)), 0.0);
pointLight3->addGlobalTransformKeyframe(getTranslationMatrix(glm::vec3(-5.0, 0.0, 2.0)), 1.0);
pointLight3->addGlobalTransformKeyframe(getTranslationMatrix(glm::vec3(5.0, 0.0, 2.0)), 2.0);
pointLight3->addGlobalTransformKeyframe(getTranslationMatrix(glm::vec3(-5.0, 0.0, 2.0)), 3.0);
viewer.addPointLight(pointLight3);
```

En lançant la scène, on voit la lumière faire des aller-retours devant les Suzannes :

#figure(
  grid(
    columns: 3,
    image("images/TP6/anim1.png"), image("images/TP6/anim2.png"), image("images/TP6/anim3.png"),
  ),
  caption: "Lumière ponctuelle animée",
)

= Exercice 6

= Conclusion
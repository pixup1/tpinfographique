#import "@preview/silky-report-insa:0.5.2": *
#show: doc => insa-report(
    id: 3,
    pre-title: "Informatique Graphique",
    title: "Modélisation",
    authors: [
        *GRAINDORGE Amance*

        *VERNET Hector*
        #v(70pt)
    ],
    date: "20/10/2025",
    insa: "rennes",
    doc,
)

= Introduction

Dans ce TP, nous découvrirons les techniques de modélisation 3D hiérarchique.
Nous commencerons par comprendre le fonctionnement de la modélisation hiérarchique et la compléter, puis nous utiliserons cette technique pour construire un arbre, avant de l'appliquer à un objet de notre projet.

= Exercice 1

Ce premier exercice consiste à comprendre le principe de la modélisation 3D hiérarchique, et à compléter deux des méthodes qui participent à son fonctionnement. En particulier, nous allons compléter les méthodes `computeTotalGlobalTransform` et `updateModelMatrix` de la classe `HierarchicalRenderable`.

La première méthode permet de calculer le total des transformations qui sont appliquées à l'objet, à partir du total des transformations appliquées à son parent, et ce récursivement jusqu'à l'objet racine :

```cpp
glm::mat4 HierarchicalRenderable::computeTotalGlobalTransform() const
{
	if (m_parent)
	{
		// Si l'objet a un parent, renvoyer les transformations de l'objet appliquées aux tranformations du parent
		return m_parent->computeTotalGlobalTransform() * m_globalTransform;
	}
	else
	{
		// Si l'objet est la racine, renvoyer les transformations de l'objet
		return m_globalTransform;
	}
}
```

La seconde méthode met simplement à jour le modèle de l'objet, qui équivaut aux transformations locales de l'objet appliquées à ses tranformations globales :

```cpp
void HierarchicalRenderable::updateModelMatrix()
{
	m_model = computeTotalGlobalTransform() * m_localTransform;
}
```

= Exercice 2

Pour ce deuxième exercice, nous allons créer un arbre récursif en utilisant la modélisation 3D hiérarchique. On utilise la méthode récursive suivante pour créer l'arbre :

```cpp
void creer_branche(int level, HierarchicalRenderablePtr parent, ShaderProgramPtr shaderProgram, float lengthMultiplier, float angle, int branching_branches, float thickness)
{
	if (level == 0)
	{
		return;
	}
	for (int i = 0; i < branching_branches; ++i) {
		// Create child
		std::shared_ptr<CylinderMeshRenderable> child = std::
			make_shared<CylinderMeshRenderable>(shaderProgram, false, 20u, false);
		// Set global transform
		glm::mat4 childGlobalTransform;
		childGlobalTransform *= getTranslationMatrix(0.0, 1.0, 0.0);
		childGlobalTransform *= getRotationMatrix(glm::radians(angle + i * (360.0f / branching_branches)), glm::vec3(0.0, 1.0, 0.0));
		childGlobalTransform *= getRotationMatrix(glm::radians(angle), glm::vec3(1.0, 0.0, 0.0));
		childGlobalTransform *= getScaleMatrix(lengthMultiplier, lengthMultiplier, lengthMultiplier);
		child->setGlobalTransform(childGlobalTransform);
		// Set local transform
		glm::mat4 childLocalTransform;
		childLocalTransform *= getScaleMatrix(thickness, 1.0, thickness);
		childLocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		child -> setLocalTransform(childLocalTransform);
		// Define parent / children relationships
		HierarchicalRenderable::addChild(parent, child);
		// Recursive call
		creer_branche(level - 1, child, shaderProgram, lengthMultiplier, angle, branching_branches, thickness);
	}
}
```

On utilise ensuite le code suivant pour intégrer l'arbre à la scène :

```cpp
	// Create root object
	std::shared_ptr<CylinderMeshRenderable> root = std ::
		make_shared<CylinderMeshRenderable>(flatShader, false, 20u, false);
	// Set root global transform
	glm::mat4 rootGlobalTransform;
	root -> setGlobalTransform(rootGlobalTransform);
	// Set root local transform
	glm::mat4 rootLocalTransform;
	rootLocalTransform *= getScaleMatrix(0.15, 1.0, 0.15);
	rootLocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	root -> setLocalTransform(rootLocalTransform);
	// Create children
	creer_branche(5, root, flatShader, 0.75f, 35.0f, 4, 0.15);
	// Add the root of the hierarchy to the viewer
	viewer.addRenderable(root);
```

#pagebreak()

Ce code nous donne le résultat suivant :

#figure(image("images/TP3/Ex2.png", width: 60%), caption: "Arbre récursif hiérarchique")

= Exercice 3

Nous allons enfin appliquer la modélisation hiérarchique à un objet de notre projet. Pour cela, nous allons utiliser le modèle de tortue que nous avons créé avec Blender. Il est composé d'une carapace, qui sera la racine du modèle, de 4 nageoires et d'une tête. De la même façon que dans l'exercice précédent, on ajoute chacun des composants dans la scène :

```cpp
void initialize_scene(Viewer& viewer)
{
	// Create a shader program
	ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(
	    "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
	    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");

	// Add the shader program to the viewer
	viewer.addShaderProgram(flatShader);

	const std::string shell_path = "../ObjFiles/Carapace.obj";
	const std::string nag_ard_path = "../ObjFiles/Nag-ArD.obj";
	const std::string nag_arg_path = "../ObjFiles/Nag-ArG.obj";
	const std::string nag_avd_path = "../ObjFiles/Nag-AvD.obj";
	const std::string nag_avg_path = "../ObjFiles/Nag-AvG.obj";
	const std::string tete_path = "../ObjFiles/Tete.obj";
	MeshRenderablePtr shell = std::make_shared<MeshRenderable>(flatShader, shell_path);
	MeshRenderablePtr nag_ard = std::make_shared<MeshRenderable>(flatShader, nag_ard_path);
	MeshRenderablePtr nag_arg = std::make_shared<MeshRenderable>(flatShader, nag_arg_path);
	MeshRenderablePtr nag_avd = std::make_shared<MeshRenderable>(flatShader, nag_avd_path);
	MeshRenderablePtr nag_avg = std::make_shared<MeshRenderable>(flatShader, nag_avg_path);
	MeshRenderablePtr tete = std::make_shared<MeshRenderable>(flatShader, tete_path);
	HierarchicalRenderable::addChild(shell, nag_ard);
	HierarchicalRenderable::addChild(shell, nag_arg);
	HierarchicalRenderable::addChild(shell, nag_avd);
	HierarchicalRenderable::addChild(shell, nag_avg);
	HierarchicalRenderable::addChild(shell, tete);
	viewer.addRenderable(shell);
	viewer.addRenderable(nag_ard);
	viewer.addRenderable(nag_arg);
	viewer.addRenderable(nag_avd);
	viewer.addRenderable(nag_avg);
	viewer.addRenderable(tete);
}
```

On obtient ainsi le résultat suivant :

#figure(image("images/TP3/Ex3.png", width: 60%), caption: "Arbre récursif hiérarchique")

= Conclusion

Dans ce TP, nous avons vu comment utiliser la modélisation hiérarchique pour créer des objects complexes à partir de composants plus simples. Nous avons utilisé cette technique pour construire un arbre récursivement et pour importer notre propre modèle conçu avec Blender. La modélisation hiérarchique sera essentielle pour l'animation, que nous aborderons dans le prochain TP et que nous utiliserons dans notre projet.
#import "@preview/silky-report-insa:0.5.2": *
#show: doc => insa-report(
    id: 2,
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

Dans ce TP, nous découvrirons les techniques de modélisation 3D paramétrique et descriptive.
Nous créerons d'abord un cylindre, puis nous verrons comment importer des modèles venant d'autres programmes, et enfin nous commencerons à modéliser la tortue pour notre projet.

= Exercice 1

Dans ce premier exercice, on commence par créer un cylindre de rayon et de hauteur 1, centré le long de l'axe z, avec la classe `CylinderMeshRenderable`.
Cette classe utilise la fonction `getUnitCylinder`, qu'il nous faut compléter à partir de l'équation d'un cylindre :

```cpp
void getUnitCylinder(vector<glm::vec3>& positions, vector<glm::vec3>& normals, std::vector<glm::vec2>& tcoords, unsigned int slices, bool vertex_normals)
{
	size_t number_of_triangles = slices * 4;              // four triangles per slice
	size_t number_of_vertices = number_of_triangles * 3;  // three vertices per triangle (unindexed version)
	float angle_step = 2.0 * M_PI / double(slices);

	positions.resize(number_of_vertices, glm::vec3(0.0, 0.0, 0.0));
	normals.resize(number_of_vertices, glm::vec3(0.0, 0.0, 0.0));
	tcoords.resize(number_of_vertices, glm::vec2(0));

	float previous_angle = (slices - 1) * angle_step;
	float angle = 0;
	float previous_cos;
	float previous_sin;
	float cos = std::cos(angle);
	float sin = std::sin(angle);

	for (size_t i = 0; i < slices; ++i)
	{
		size_t voffset = 12 * i;  // 4 x 3 = 12 vertices per slice
		previous_angle = angle;
		angle += angle_step;
		previous_cos = cos;
		previous_sin = sin;
		cos = std::cos(angle);
		sin = std::sin(angle);

		// Positions

		// top triangle
		positions[voffset + 0] = glm::vec3(0.0, 0.0, 1.0);
		positions[voffset + 1] = glm::vec3(cos, sin, 1.0);
		positions[voffset + 2] = glm::vec3(previous_cos, previous_sin, 1.0);

		// side triangles
		positions[voffset + 3] = glm::vec3(cos, sin, 1.0);
		positions[voffset + 4] = glm::vec3(previous_cos, previous_sin, 1.0);
		positions[voffset + 5] = glm::vec3(previous_cos, previous_sin, 0.0);
		positions[voffset + 6] = glm::vec3(previous_cos, previous_sin, 0.0);
		positions[voffset + 7] = glm::vec3(cos, sin, 0.0);
		positions[voffset + 8] = glm::vec3(cos, sin, 1.0);

		// bottom triangle
		positions[voffset + 9] = glm::vec3(0.0, 0.0, 0.0);
		positions[voffset + 10] = glm::vec3(cos, sin, 0.0);
		positions[voffset + 11] = glm::vec3(previous_cos, previous_sin, 0.0);

		// Normals

		// Texture coordinates
	}
}
```

Le cylindre renvoyé par cette fonction est approximé en un certain nombre de parts (`slices`), composées d'un triangle pour la face supérieure, un pour la face inférieure, et deux qui forment le rectangle du côté.
On complète également le constructeur de la classe `CylinderMeshRenderable` pour utiliser cette fonction et donner une couleur différente à chaque face du cylindre :

```cpp
CylinderMeshRenderable::CylinderMeshRenderable(ShaderProgramPtr shaderProgram, bool indexed, unsigned int slices, bool vertex_normals) : MeshRenderable(shaderProgram, indexed)
{
	getUnitCylinder(m_positions, m_normals, m_tcoords, slices, vertex_normals);
	m_colors.resize(m_positions.size(), glm::vec4(0));
	for (size_t i = 0; i < m_colors.size() / 12; ++i)
	{
		glm::vec4 topColor = glm::vec4(1, 0, 0, 1);
		glm::vec4 botColor = glm::vec4(0, 1, 0, 1);
		glm::vec4 latColor = glm::vec4(0, 0, 1, 1);

		// Top triangle color
		m_colors[12 * i + 0] = topColor;
		m_colors[12 * i + 1] = topColor;
		m_colors[12 * i + 2] = topColor;

		// Side triangles color
		m_colors[12 * i + 3] = latColor;
		m_colors[12 * i + 4] = latColor;
		m_colors[12 * i + 5] = latColor;
		m_colors[12 * i + 6] = latColor;
		m_colors[12 * i + 7] = latColor;
		m_colors[12 * i + 8] = latColor;

		// Bottom triangle color
		m_colors[12 * i + 9] = botColor;
		m_colors[12 * i + 10] = botColor;
		m_colors[12 * i + 11] = botColor;
	}
	update_all_buffers();
}
```
#pagebreak()

On obtient ainsi ce rendu :

#figure(image("images/TP2/Ex1.png", width: 60%), caption: "Cylindre sans normales")

= Exercice 2

Ce second exercice ajoute le calcul des normales du cylindre selon deux méthodes : par point ou par triangle.
Cet ajout se fait en modifiant la fonction `getUnitCylinder` du fichier `Utils.cpp` :

```cpp
// Normals

// top triangle
normals[voffset + 0] = glm::vec3(0.0, 1.0, 0.0);
normals[voffset + 1] = glm::vec3(0.0, 1.0, 0.0);
normals[voffset + 2] = glm::vec3(0.0, 1.0, 0.0);

// side triangles
if (vertex_normals)
{
	// Per vertex normals
	normals[voffset + 3] = glm::vec3(cos, sin, 0.0);
	normals[voffset + 4] = glm::vec3(previous_cos, previous_sin, 0.0);
	normals[voffset + 5] = glm::vec3(previous_cos, previous_sin, 0.0);
	normals[voffset + 6] = glm::vec3(previous_cos, previous_sin, 0.0);
	normals[voffset + 7] = glm::vec3(cos, sin, 1.0);
	normals[voffset + 8] = glm::vec3(cos, sin, 1.0);
}
else
{
	// Per triangle normals
	glm::vec3 n = glm::normalize(glm::vec3((cos + previous_cos) / 2, (sin + previous_sin) / 2, 0.0));
	normals[voffset + 3] = n;
	normals[voffset + 4] = n;
	normals[voffset + 5] = n;
	normals[voffset + 6] = n;
	normals[voffset + 7] = n;
	normals[voffset + 8] = n;
}

normals[voffset + 9] = glm::vec3(0.0, -1.0, 0.0);
normals[voffset + 10] = glm::vec3(0.0, -1.0, 0.0);
normals[voffset + 11] = glm::vec3(0.0, -1.0, 0.0);
```

On change également le vertex shader (`flatVertex.glsl`) pour changer la couleur du point en fonction de sa normale :

```glsl
uniform mat4 projMat, viewMat, modelMat;

in vec3 vPosition;
in vec3 vNormal;
out vec4 surfel_color;

void main()
{
    gl_Position = projMat*viewMat*modelMat*vec4(vPosition, 1.0f);
    int w = 1;
    vec3 cNormal = vec3(vNormal[0] * 0.5 + 0.5, vNormal[1] * 0.5 + 0.5, vNormal[2] * 0.5 + 0.5); // Change normal vector to stay in [0,1]
    surfel_color = vec4(cNormal, w);
}
```

Cela nous donne le résultat suivant :

#figure(image("images/TP2/Ex2.png", width: 60%), caption: "Cylindre avec normales dans le vertex shader")

= Exercice 3

Pour ce troisième exercice, nous devons importer des mesh stockés dans des fichiers : `cat.obj` et `pillar.obj`.
Nous devons également utiliser les transformations pour que l'objet `cat` soit positionné sur l'objet `pillar`.
On modifie donc le fichier `practical2.cpp` :

```cpp
const std::string cat_path = "../../sfmlGraphicsPipeline/meshes/cat.obj";
const std::string pillar_path = "../../sfmlGraphicsPipeline/meshes/pillar.obj";
// Import both objects
MeshRenderablePtr cat = std::make_shared<MeshRenderable>(flatShader, cat_path);
MeshRenderablePtr pillar = std::make_shared<MeshRenderable>(flatShader, pillar_path);

// Move the cat up
cat->setModelMatrix(getTranslationMatrix(0.5,3.2,0));
// Rotate the pillar
pillar->setModelMatrix(getRotationMatrix(glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0)));

viewer.addRenderable(cat);
viewer.addRenderable(pillar);
```

Pour que les couleurs données par les normales soient cohérentes entre les deux objets, il faut modifier le vertex shader pour utiliser les coordonnées globales pour les normales.

```glsl
gl_Position = projMat*viewMat*modelMat*vec4(vPosition, 1.0f);
vec3 norm = normalize(transpose(inverse(mat3(modelMat))) * vNormal); // Change to global coordinates
int w = 1;
vec3 cNormal = vec3(norm[0] * 0.5 + 0.5, norm[1] * 0.5 + 0.5, norm[2] * 0.5 + 0.5);
surfel_color = vec4(cNormal, w);
```

On obtient ainsi des couleurs uniforme sur les deux objets :

#figure(image("images/TP2/Ex3.png", width: 60%), caption: "Chat sur un arbre à chat avec normales dans le vertex shader")


= Exercice 4

Cet exercice consiste à commencer la modélisation 3D pour notre projet. Pour cela, nous modéliserons les différents éléments dans Blender. Nous pourrons plus tard les importer dans notre projet au format Wavefront (`.obj`) comme vu plus haut.

Notre scénario (nouvellement écrit) mettant en scène une tortue de mer ainsi qu'une plage, nous commencerons par modéliser cela.

== Tortue

Pour modéliser la tortue, nous utiliserons le workflow dit "subsurf" (Subdivision Surface), qui permet de subdiviser des formes basiques composées de quadrilatères pour obtenir un résultat plus détaillé et organique.

#figure(image("images/TP2/Ex4-1.png"), caption: "Notre modèle de tortue dans Blender (mesh après subdivision)")

Pour ce qui est des textures, nous verrons ça plus tard.

== Plage

Pour ce qui est de la plage, on peut modéliser le terrain en sculptant un plan subdivisé. Pour l'océan, un modificateur "Océan" existe dans Blender, ce qui permet de générer des vagues automatiquement. Enfin, pour les palmiers, on utilise une texture de feuille trouvée sur internet. Nous verrons comment appliquer des textures dans notre projet à l'occasion du TP7, à priori.

#figure(image("images/TP2/Ex4-2.png"), caption: "Modélisation de la tortue dans Blender (mesh après subdivision)")

Tout comme la tortue, ce modèle n'est évidemment pas terminé. On envisage notamment de rajouter du corail et des rochers sous l'océan afin de détailler le fond marin, puisqu'une partie de notre scénario s'y déroule.

= Conclusion

Dans ce TP, nous avons vu comment utiliser la modélisation paramétrique pour créer des primitives comme un cylindre, puis comment utiliser un logiciel comme Blender pour modéliser des objets plus complexes (modélisation descriptive). Nous avons également vu comment différents logiciels peuvent se transmettre des modèles 3D via des formats standards comme le Wavefront. Nous sommes donc prêts à continuer la modélisation complète de notre scène pour le projet.
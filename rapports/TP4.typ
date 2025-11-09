#import "@preview/silky-report-insa:0.5.2": *
#show: doc => insa-report(
  id: 4,
  pre-title: "Informatique Graphique",
  title: "Animation procédurale",
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

Au cours de ce TP, nous nous attelerons à compléter et étender le code d'animation présent dans la pipeline fournie. Ce code aura pour but de pouvoir définir des animations de modèles à partir de _keyframes_ (ou images clés) représentant la transformation d'un objet à un point dans le temps. Une fois que les fonctions de base fonctionneront, nous pourrons les améliorer pour notre projet avec des fonctionnalités telles que l'importation depuis un logiciel d'animation, et d'autres méthodes d'interpolation.

= Exercice 1

Dans cet exercice, nous allons tout d'abord finir la fonction `interpolateTransformation()` de la classe `KeyframeCollection` qui a pour but de donner la transformation d'un objet à tout instant t, en prenant en compte les deux keyframes l'entourant.

Voici le code de la fonction terminée :

```cpp
// Handle the case where the time parameter is outside the keyframes time scope.
std::map<float, GeometricTransformation>::const_iterator itFirstFrame = m_keyframes.begin();
std::map<float, GeometricTransformation>::const_reverse_iterator itLastFrame = m_keyframes.rbegin();
float effective_time = std::fmod(time, itLastFrame->first);

// Get keyframes surrounding the time parameter
std::array<Keyframe, 2> result = getBoundingKeyframes(effective_time);

float factor = (time - result[0].first) / (result[1].first - result[0].first);

glm::vec3 interpTranslation = glm::lerp(result[0].second.getTranslation(), result[1].second.getTranslation(), factor);
glm::vec3 interpScale = glm::lerp(result[0].second.getScale(), result[1].second.getScale(), factor);
glm::quat interpOrientation = glm::slerp(glm::normalize(result[0].second.getOrientation()), glm::normalize(result[1].second.getOrientation()), factor);
glm::mat4 iMatrix(1.0f);

iMatrix = glm::translate(iMatrix, interpTranslation);
iMatrix *= glm::toMat4(interpOrientation);
iMatrix = glm::scale(iMatrix, interpScale);
iMatrix = glm::scale(iMatrix, interpScale);

return iMatrix;
```

On insterpole chaque composante de la transformation séparément avant de les combiner, afin de conserver la forme du modèle.

Une fois que cette fonction marche, on peut s'en servir dans le fichier `practical4.cpp`. On commence par la fonction `movingCylinder`
 :

```cpp
// Add shader
ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
viewer.addShaderProgram(flatShader);

// Frame
FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
viewer.addRenderable(frame);

// Animated cylinder
auto cylinder = std::make_shared<CylinderMeshRenderable>(flatShader, false, 20, false);
cylinder->setGlobalTransform(glm::mat4(1.0));

cylinder->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::quat(),
                               glm::vec3(1.0f, 1.0f, 1.0f)),
                   0.0f);
cylinder->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::angleAxis(M_PIf * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f)),
                               glm::vec3(1.0f, 1.0f, 1.0f)),
                   5.0f);

cylinder->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, -5.0f),
                              glm::quat(),
                              glm::vec3(1.0f, 1.0f, 1.0f)),
                  0.0f);
cylinder->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0.0f, 0.0f, -5.0f),
                              glm::angleAxis(M_PIf * 1.5f, glm::vec3(0.0f, 0.0f, 1.0f)),
                              glm::vec3(1.0f, 1.0f, 1.0f)),
                  5.0f);

viewer.addRenderable(cylinder);
viewer.startAnimation();
```

En plaçant des keyframes à t=0 et 5 secondes, on définit un cylindre qui fait une rotation de 270° autour de l'axe Y global, et une rotation de 270° autour de son axe local Z en 5 secondes (pour donner l'impression qu'il roule).

Ensuite, on s'occupe de la fonction `movingTree`, qui doit générer un arbre animé. Pour cela, on modifie la fonction `creer_branche` que nous avions écrite pour le TP précédent :

```cpp
void creer_branche(int level, HierarchicalRenderablePtr parent, ShaderProgramPtr shaderProgram, float lengthMultiplier, float angle, int branching_branches, float thickness)
{
    if (level == 0)
    {
        return;
    }
    for (int i = 0; i < branching_branches; ++i)
    {
        // Create child
        auto child = std::make_shared<CylinderMeshRenderable>(shaderProgram, false, 20u, false);
        // Set global transform
        child->setGlobalTransform(glm::mat4(1.0));
        
        glm::mat4 childGlobalTransform;
        childGlobalTransform *= getTranslationMatrix(0.0, 1.0, 0.0);
        childGlobalTransform *= getRotationMatrix(glm::radians(angle + i * (360.0f / branching_branches)), glm::vec3(0.0, 1.0, 0.0));
        childGlobalTransform *= getRotationMatrix(glm::radians(angle), glm::vec3(1.0, 0.0, 0.0));
        childGlobalTransform *= getScaleMatrix(lengthMultiplier, lengthMultiplier, lengthMultiplier);

        childGlobalTransform *= getRotationMatrix(glm::radians(10.0f), glm::vec3(1.0, 0.0, 0.0));
        child->addGlobalTransformKeyframe(childGlobalTransform, 0.0f);
        child->addGlobalTransformKeyframe(childGlobalTransform, 5.0f);
        
        childGlobalTransform *= getRotationMatrix(glm::radians(-10.0f), glm::vec3(1.0, 0.0, 0.0));
        child->addGlobalTransformKeyframe(childGlobalTransform, 2.5f);

        // Set local transform
        glm::mat4 childLocalTransform;
        childLocalTransform *= getScaleMatrix(thickness, 1.0, thickness);
        childLocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        child->setLocalTransform(childLocalTransform);
        // Define parent / children relationships
        HierarchicalRenderable::addChild(parent, child);
        // Recursive call
        creer_branche(level - 1, child, shaderProgram, lengthMultiplier, angle, branching_branches, thickness);
    }
}

void movingTree(Viewer &viewer)
{
    // Add shader
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>("../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                  "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    // Frame
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    std::shared_ptr<CylinderMeshRenderable> root = std ::
        make_shared<CylinderMeshRenderable>(flatShader, false, 20u, false);
    
    glm::mat4 rootLocalTransform;
    rootLocalTransform *= getScaleMatrix(0.15, 1.0, 0.15);
    rootLocalTransform *= getRotationMatrix(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    root->setLocalTransform(rootLocalTransform);

    creer_branche(5, root, flatShader, 0.75f, 35.0f, 4, 0.15);

    viewer.addRenderable(root);

    viewer.startAnimation();
}
```

Cela a pour résultat de faire bouger les branches de haut en bas (ce qui correspond à une rotation autour de l'axe X des branches). On peut jouer l'animation en boucle avec `viewer.setAnimationLoop(true, 5.0)`.

= Exercice 2

À présent, nous allons voir des améliorations possibles à apporter au système d'animation au delà de ce qui est suggéré.

== Interpolation

Actuellement, nous utilisons une interpolation linéaire pour les transformations. Cependant, il existe d'autres méthodes qui peuvent donner des animations plus fluides pour le même nombre de keyframes.

En se basant sur #link("https://graphicscompendium.com/opengl/22-interpolation")[https://graphicscompendium.com/opengl/22-interpolation], nous allons donc implémenter l'interpolation cubique, pour remplacer la fonction `glm::lerp`. Nous allons aussi changer le modèle de données des keyframes, afin de stocker le type d'interpolation.

```cpp
float cubicInterpolate(
    float x0, float x1,
    float x2, float x3,
    float t)
{
    float a = (3.0 * x1 - 3.0 * x2 + x3 - x0) / 2.0;
    float b = (2.0 * x0 - 5.0 * x1 + 4.0 * x2 - x3) / 2.0;
    float c = (x2 - x0) / 2.0;
    float d = x1;

    return a * t * t * t +
           b * t * t +
           c * t +
           d;
}
```

Ensuite, pour appeler cette fonction, on fait un switch sur le type d'interpolation stocké dans la keyframe :

```cpp
// Handle the case where the time parameter is outside the keyframes time scope.
std::map<float, Keyframe>::const_iterator first = m_keyframes.begin();
std::map<float, Keyframe>::const_iterator last = std::prev(m_keyframes.end());
if (time <= first->first) {
    return first->second.transform.toMatrix();
} else if (time >= last->first) {
    return last->second.transform.toMatrix();
}

glm::mat4 iMatrix(1.0f);

std::map<float, Keyframe>::const_iterator ki2 = m_keyframes.upper_bound(time);
std::map<float, Keyframe>::const_iterator ki1 = std::prev(ki2);

float factor = (time - ki1->first) / (ki2->first - ki1->first);

switch (ki1->second.interpolation)
{
    case CUBIC: {
        std::map<float, Keyframe>::const_iterator first = m_keyframes.begin();
        std::map<float, Keyframe>::const_iterator last = m_keyframes.end();
        
        std::map<float, Keyframe>::const_iterator ki0;
        std::map<float, Keyframe>::const_iterator ki3 = std::next(ki2);
        if (ki1 == first) {
            ki0 = ki1;
        } else {
            ki0 = std::prev(ki1);
        }
        if (ki3 == last) {
            ki3 = ki2;
        }
        GeometricTransformation g0 = ki0->second.transform;
        GeometricTransformation g1 = ki1->second.transform;
        GeometricTransformation g2 = ki2->second.transform;
        GeometricTransformation g3 = ki3->second.transform;
        glm::vec3 t0 = g0.getTranslation();
        glm::vec3 t1 = g1.getTranslation();
        glm::vec3 t2 = g2.getTranslation();
        glm::vec3 t3 = g3.getTranslation();
        glm::vec3 s0 = g0.getScale();
        glm::vec3 s1 = g1.getScale();
        glm::vec3 s2 = g2.getScale();
        glm::vec3 s3 = g3.getScale();
        glm::quat r0 = glm::normalize(g0.getOrientation());
        glm::quat r1 = glm::normalize(g1.getOrientation());
        glm::quat r2 = glm::normalize(g2.getOrientation());
        glm::quat r3 = glm::normalize(g3.getOrientation());
        glm::vec3 interpTranslation;
        glm::vec3 interpScale;
        glm::quat interpOrientation;
        for (int i = 0; i < 3; i++) {
            interpTranslation[i] = cubicInterpolate(
                t0[i], t1[i], t2[i], t3[i],
                factor
            );
            interpScale[i] = cubicInterpolate(
                s0[i], s1[i], s2[i], s3[i],
                factor
            );
        }
        for (int i = 0; i < 4; i++) {
            interpOrientation[i] = cubicInterpolate(
                r0[i], r1[i], r2[i], r3[i],
                factor);
        }
        interpOrientation = glm::normalize(interpOrientation);

        iMatrix = glm::translate(iMatrix, interpTranslation);
        iMatrix *= glm::toMat4(interpOrientation);
        iMatrix = glm::scale(iMatrix, interpScale);

        break;
    }
    case LINEAR: {
        Keyframe k1 = ki1->second;
        Keyframe k2 = ki2->second;

        glm::vec3 interpTranslation = glm::lerp(k1.transform.getTranslation(), k2.transform.getTranslation(), factor);
        glm::vec3 interpScale = glm::lerp(k1.transform.getScale(), k2.transform.getScale(), factor);
        glm::quat interpOrientation = glm::slerp(glm::normalize(k1.transform.getOrientation()), glm::normalize(k2.transform.getOrientation()), factor);

        iMatrix = glm::translate(iMatrix, interpTranslation);
        iMatrix *= glm::toMat4(interpOrientation);
        iMatrix = glm::scale(iMatrix, interpScale);

        break;
    }
    case CONSTANT: {
        iMatrix = ki1->second.transform.toMatrix();

        break;
    }
}

return iMatrix;
```

On a aussi ajouté le mode d'interpolation CONSTANT, qui ne correspond à aucune interpolation, et qui est donc trivial à implémenter.

== Importation d'animation depuis Blender

Notre système d'animation a désormais le minimum syndical de fonctionnalités pour produire une animation correcte. Cependant, entrer les valeurs des keyframes à la main est une technique qui était utilisée dans les années 80, mais des outils bien plus faciles d'utilisations existent aujourd'hui, et nous permettraient de gagner beaucoup de temps.

Pour cela, nous utiliserons Blender, dont nous nous servons déjà pour la modélisation de nos objets. Nous exportons les modèles au format Wavefront, qui est assez basique et ne contient que des données sur le mesh. Pour l'animation, les formats la supportant sont plus complexes et donc plus difficiles à implémenter. Nous créerons donc notre propre format.

Pour gérer tout cela, vu que Blender a une API Python très complète (`bpy`), nous allons écrire un script qui pourra exporter tous les objets d'une scène ainsi que leur animation. Le voici :

```python
# pyright: reportOptionalMemberAccess=false

import bpy
import mathutils

abs_path = bpy.path.abspath("//")
export_path = f"{abs_path}/../sampleProject/ObjFiles"

def frame_to_time(frame: int):
    return (frame - bpy.context.scene.frame_start) / bpy.context.scene.render.fps

def export_mesh(obj):
    for ob in bpy.data.objects:
        ob.select_set(False)
    obj.select_set(True)
    backup_matrix = obj.matrix_world.copy()
    obj.matrix_world = mathutils.Matrix.Identity(4)
    bpy.ops.wm.obj_export(
        filepath=f"{export_path}/{obj.name}.obj",
        apply_modifiers=True,
        export_selected_objects=True,
        export_materials=False
    )
    y_up_rot = mathutils.Matrix((
        (1, 0, 0, 0),
        (0, 0, 1, 0),
        (0, -1, 0, 0),
        (0, 0, 0, 1)
    ))
    y_up_matrix = y_up_rot @ backup_matrix @ y_up_rot.inverted()
    with open(f"{export_path}/{obj.name}.obj", "a") as f:
        f.write("\nTRANSFORM")
        for row in y_up_matrix:
            for val in row:
                f.write(f" {val}")
        f.write("\n")
    obj.matrix_world = backup_matrix
    obj.select_set(False)

def export_animation(obj):
    output = ""
    if obj.animation_data and obj.animation_data.action:
        frames = {}
        for fcurve in obj.animation_data.action.fcurves:
            for keyframe in fcurve.keyframe_points:
                if int(keyframe.co.x) not in frames:
                    frames[int(keyframe.co.x)] = [keyframe.interpolation]
                else:
                    frames[int(keyframe.co.x)].append(keyframe.interpolation)
        scene = bpy.context.scene
        depsgraph = bpy.context.evaluated_depsgraph_get()
        for fr in sorted(frames.keys()):
            scene.frame_set(fr)
            eval_obj = obj.evaluated_get(depsgraph)
            loc, rot, scale = eval_obj.matrix_world.decompose()
            t = frame_to_time(fr)
            most_frequent_interp = max(set(frames[fr]), key=frames[fr].count)
            output += f"{t},{most_frequent_interp},{loc.x},{loc.y},{loc.z},{rot.x},{rot.y},{rot.z},{rot.w},{scale.x},{scale.y},{scale.z}\n"
        if output != "":
            with open(f"{export_path}/{obj.name}.animation", "w") as f:
                f.write(output)

if bpy.context.scene is not None:
    for obj in bpy.data.objects:
        export_mesh(obj)
        export_animation(obj)
```

Ce script rajoute également une propriété `TRANSFORM` au fichier `.obj`, qui permet de partager la transformation globale appliquée aux objets dans Blender. En effet, normalement, le format Wavefront n'inclut que la position absolue des points après transformation.

Après exécution, on se retrouve avec un fichier `.obj` et un fichier `.animation` par objet. Pour parser ces fichiers, nous écrivons la méthode suivante :

```cpp
void KeyframeCollection::addFromFile(const std::string &animation_filename, float time_shift) {
    std::ifstream is(animation_filename);
    std::string str;
    while (getline(is, str))
    {
        std::regex regexz(",");
        std::vector<std::string> split(std::sregex_token_iterator(str.begin(), str.end(), regexz, -1),
                                      std::sregex_token_iterator());
        // Don't forget to convert to Y-up !
        glm::vec3 loc = glm::vec3(std::stof(split[2]), std::stof(split[4]), -std::stof(split[3]));
        glm::vec3 size = glm::vec3(std::stof(split[9]), std::stof(split[11]), std::stof(split[10]));
        glm::quat rot = glm::quat(std::stof(split[8]), std::stof(split[5]), std::stof(split[7]), -std::stof(split[6]));
        
        // Default interpolation is cubic
        KeyframeInterpolationMode interp = CUBIC;
        if (split[1] == "LINEAR") {
            interp = LINEAR;
        } else if (split[1] == "CONSTANT") {
            interp = CONSTANT;
        } else if (split[1] == "CUBIC") {
            interp = CUBIC;
        }

        this->add(GeometricTransformation(loc, glm::normalize(rot), size), std::stof(split[0]) + time_shift, interp);
    }
}
```

Nous avons du apporter des modifications à d'autres fichiers, notamment pour gérer les nouvelles données de transformation dans le `.obj`, et pour stocker une matrice inverse lorsqu'on rajoute un parent à un `HierarchicalRenderable`, afin d'avoir une hiérarchie de transformations fonctionnelle. Mais ce rapport est déjà assez long, nous n'irons pas dans les détails. Vous pouvez aller voir ces deux commits :

- #link("https://github.com/pixup1/tpinfographique/commit/8d7807f03bac0b80625253ebc1c0289da87e0284")
- #link("https://github.com/pixup1/tpinfographique/commit/d42c34f690a0589d573c4d8046d1d9b112fae514")

= Conclusion

En conclusion, lors de ce TP, nous avons implémenté un système d'animation procédurale basé sur des keyframes, avec différentes méthodes d'interpolation, et un outil d'exportation depuis Blender. Cela nous permettra de réaliser notre projet plus rapidement et avec des animations moins rudimentaires.
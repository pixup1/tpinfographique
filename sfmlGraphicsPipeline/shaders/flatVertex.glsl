#version 400

uniform mat4 projMat, viewMat, modelMat;

in vec3 vPosition;
in vec3 vColor;
out vec4 surfel_color;

void main()
{
    gl_Position = projMat*viewMat*modelMat*vec4(vPosition, 1.0f);
    int w = 1;
    surfel_color = vec4(vColor, w);
}

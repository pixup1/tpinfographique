#version 400

uniform mat4 projMat, viewMat, modelMat;

in vec3 vPosition;
in vec3 vNormal;
out vec4 surfel_color;

void main()
{
    gl_Position = projMat*viewMat*modelMat*vec4(vPosition, 1.0f);
    int w = 1;
    vec3 cNormal = vec3(vNormal[0] * 0.5 + 0.5, vNormal[1] * 0.5 + 0.5, vNormal[2] * 0.5 + 0.5);
    surfel_color = vec4(cNormal, w);
}

#version 400

uniform mat4 projMat, viewMat, modelMat;

in vec3 vPosition;
in vec3 vNormal;
in vec4 instanceData;
out vec4 surfel_color;

void main()
{
    vec3 p = instanceData.xyz + vPosition * instanceData.w;
    gl_Position = projMat*viewMat*modelMat*vec4(p, 1.0f);
    surfel_color = vec4(150.0/255.0, 50.0/255.0, 50.0/255.0, 1.0);
}

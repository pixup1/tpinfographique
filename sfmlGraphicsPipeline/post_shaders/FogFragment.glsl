#version 400

uniform sampler2D fbo_texture;
uniform sampler2D fbo_depth;
varying vec2 f_texcoord;

void main(void) {
  float depth = texture2D(fbo_depth, f_texcoord).r;
  gl_FragColor = vec4(vec3(depth), 1.0);
}

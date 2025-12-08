uniform sampler2D texture;

void main()
{
    vec4 c = texture2D(texture, gl_TexCoord[0].xy);
    float g = dot(c.rgb, vec3(0.299, 0.587, 0.114));
    gl_FragColor = vec4(g, g, g, 1.0);
}

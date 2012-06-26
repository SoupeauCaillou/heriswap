#ifdef GL_ES
precision lowp float;
#endif
 // color sampler
uniform sampler2D tex0;
 // alpha sampler
uniform sampler2D tex1;
uniform vec4 vColor;

varying vec2 uvVarying;

void main()
{
    vec4 col = vec4(texture2D(tex0, uvVarying).rgb, 1.0) * vColor;
    float sum = (col.r * 0.333 + col.g * 0.333 + col.b * 0.333);
    gl_FragColor.rgb = vec3(sum);
    gl_FragColor.a = texture2D(tex1, uvVarying).a * vColor.a;
}

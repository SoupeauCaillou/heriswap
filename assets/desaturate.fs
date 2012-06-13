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
    gl_FragColor = texture2D(tex0, uvVarying) * vColor;
    float sum = (gl_FragColor.r + gl_FragColor.g + gl_FragColor.b) / 3.0;
    gl_FragColor.rgb = vec3(sum);
    gl_FragColor.a = texture2D(tex1, uvVarying).a * vColor.a;
}

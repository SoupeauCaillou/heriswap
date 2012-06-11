#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D tex0; // color sampler
uniform sampler2D tex1; // alpha sampler

varying vec2 uvVarying;
varying vec4 vColor;

void main()
{
    gl_FragColor = texture2D(tex0, uvVarying) * vColor;
    float sum = (gl_FragColor.r + gl_FragColor.g + gl_FragColor.b) / 3.0f;
    gl_FragColor.rgb = vec3(sum);
    gl_FragColor.a = texture2D(tex1, uvVarying).a * vColor.a;
}
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
    gl_FragColor.a = texture2D(tex1, uvVarying).a * vColor.a;
}

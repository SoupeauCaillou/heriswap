#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D tex0;

varying vec2 uvVarying;
varying vec4 vColor;

void main()
{
    gl_FragColor = texture2D(tex0, uvVarying) * vColor;
}


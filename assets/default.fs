#ifdef GL_ES
precision lowp float;
#endif
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec4 vColor;

varying vec2 uvVarying;

void main()
{
    gl_FragColor = texture2D(tex0, uvVarying) * vColor;
    gl_FragColor.a = texture2D(tex1, uvVarying).a * vColor.a;
}


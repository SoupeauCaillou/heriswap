#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D tex0;
uniform int useTexturing;

varying vec2 uvVarying;
varying vec4 vColor;

void main()
{
	if (useTexturing > 0)
   	gl_FragColor = texture2D(tex0, uvVarying) * vColor;
   else
   	gl_FragColor = vColor;
}

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
	vec3 color =
		texture2D(tex0, uvVarying).rgb * vColor.rgb;

    float sum = (color.r * 0.333 + color.g * 0.333 + color.b * 0.333);
    gl_FragColor.rgb = vec3(sum);
    gl_FragColor.a = texture2D(tex1, uvVarying).r * vColor.a;
}

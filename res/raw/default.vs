attribute vec3 aPosition;
attribute vec2 aTexCoord; 
attribute vec4 aColor;

uniform mat4 uMvp;

varying vec4 vColor;
varying vec2 uvVarying;

void main()
{
	vec4 position = uMvp * vec4(aPosition.xyz, 1.);
	vColor = aColor;
	gl_Position = position;
	uvVarying = aTexCoord;
}


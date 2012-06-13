attribute vec3 aPosition;
attribute vec2 aTexCoord; 

uniform mat4 uMvp;

varying vec2 uvVarying;

void main()
{	
	gl_Position = uMvp * vec4(aPosition, 1.0);
	uvVarying = aTexCoord;
}


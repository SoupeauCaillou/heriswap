attribute vec3 aPosition;
attribute vec4 aPosRot;
attribute vec2 aTexCoord; 
attribute vec4 aColor;

uniform mat4 uMvp;

varying vec4 vColor;
varying vec2 uvVarying;

void main()
{
	mat3 rotation = mat3(
        vec3( cos(aPosRot.w),  sin(aPosRot.w),  0.0),
        vec3(-sin(aPosRot.w),  cos(aPosRot.w),  0.0),
        vec3(        0.0,         0.0,  1.0)
    );
	
	vec4 position = uMvp *  vec4(rotation * aPosition.xyz + vec3(aPosRot.xy, -aPosRot.z), 1.);
	vColor = aColor;
	gl_Position = position;
	uvVarying = aTexCoord;
}


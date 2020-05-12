#version 330

smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;

uniform int bTextured;
uniform int bLighted;
uniform int bFog;

uniform int numSplats;
uniform sampler2D gSplatMaps[8];

uniform sampler2D gSampler[8];
uniform vec2 uv_repeats[8];

uniform vec4 vColor;

varying vec2 uv_splat;
varying vec2 uv_scale;

out vec4 outputColor;

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbient;
};


uniform DirectionalLight sunLight;

uniform struct FogParameters
{
	vec4 vFogColor; 
	float fStart; 
	float fEnd; 
	float fDensity; 	
	int iEquation; 
} fogParams;

float getFogFactor(FogParameters params, float fFogCoord);

void main()
{

	vec3 color = vec3(1.0, 1.0, 1.0); 

	if ( bTextured == 1 ) {

		color = vec3(0.0, 0.0, 0.0); 

	for ( int i = 0; i < numSplats; i++ ) {

		vec4 a = texture2D(gSplatMaps[i], uv_splat.st).rgba;
	
		vec3 d0 = texture2D(gSampler[0*(i+1)], uv_repeats[0*(i+1)].st * uv_scale).rgb; 
    		vec3 d1 = texture2D(gSampler[1*(i+1)], uv_repeats[1*(i+1)].st * uv_scale).rgb;
    		vec3 d2 = texture2D(gSampler[2*(i+1)], uv_repeats[2*(i+1)].st * uv_scale).rgb;
    		vec3 d3 = texture2D(gSampler[3*(i+1)], uv_repeats[3*(i+1)].st * uv_scale).rgb;	

    		d0 *= a.r;
    		d1 = mix(d0,  d1, a.g);
    		d2 = mix(d1, d2, a.b);
    		color += mix(d2, d3, a.a);

	}

	}

	outputColor = vec4(color / numSplats, 1.0) * vColor;

	if ( bLighted == 1) {
			
		float DiffuseIntensity = max(0.0, dot(normalize(vNormal), -sunLight.vDirection));
		outputColor = vec4(color / numSplats, 1.0) * vColor * vec4(sunLight.vColor* (sunLight.fAmbient+ DiffuseIntensity), 1.0);
	}

	if ( bFog == 1 ) {

		float fFogCoord = abs(vEyeSpacePos.z/vEyeSpacePos.w);
		outputColor = mix(outputColor, fogParams.vFogColor, getFogFactor(fogParams, fFogCoord));

	}
  
}
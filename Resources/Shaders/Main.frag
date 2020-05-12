#version 330

in vec3 Normal;
in vec2 TexCoord;
in vec4 Position;
in vec4 vEyeSpacePos;

out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

uniform int bTextured;
uniform int bLighted;
uniform int bFog;

uniform vec4 LightPos;
uniform vec4 LightColor;
uniform vec4 AmbientColor;
uniform vec4 MatAmbient;
uniform vec4 MatDiffuse;
uniform vec4 MatSpecular;
uniform vec4 Emissive;
uniform float Shininess;
uniform vec4 CameraPos;

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

	vec4 Color = vec4(1.0, 1.0, 1.0, 1.0); 

	if (bTextured == 1) {

		vec4 vTexColor = texture2D(gSampler, TexCoord);
		vec4 vMixedColor = vTexColor*vColor;
		
		Color = vTexColor*vMixedColor;
	}

outputColor = Color;

	if (bLighted == 1) {

   		vec3 N = normalize(Normal);
   
		vec4 Ambient = MatAmbient * AmbientColor;

   		vec4 LightVec = normalize(LightPos - Position);
   		float DiffuseLight = max(dot(N, vec3(LightVec)), 0.0);
   		vec4 Diffuse = MatDiffuse * LightColor * DiffuseLight;

   		vec4 EyeVec = normalize(CameraPos - Position);
   		vec4 HalfVec = normalize(LightVec + EyeVec );
   		float SpecularLight = pow(max(dot(N, vec3(HalfVec)), 0), Shininess);

   		if(DiffuseLight <= 0) SpecularLight = 0;
   		vec4 Specular = MatSpecular * LightColor * SpecularLight;
   
   		outputColor = (Emissive + Ambient + Diffuse + Specular) * Color;
	} 

	if ( bFog == 1 ) {

		float fFogCoord = abs(vEyeSpacePos.z/vEyeSpacePos.w);
		outputColor = mix(outputColor, fogParams.vFogColor, getFogFactor(fogParams, fFogCoord));

	}

}
